package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import ioio.TimerUtil;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.IOIOLooperProvider;

public class IOService implements IOIOLooperProvider  {
  private static final String TAG = "IOService";
  private static final int MAX_PINS = 46;
  private static IOService instance = null;

  private final ConnectionController connectionController;
  private final Boolean[] usedPins;
  private final IOServiceLooper looper;
  private final List<IOTask> ioTasks;

  private IOService() {
    connectionController = new ConnectionController(this);
    looper = new IOServiceLooper();
    ioTasks = new ArrayList<>();
    usedPins = new Boolean[MAX_PINS + 1];
  }

  public static IOService getInstance() {
    if (instance == null) {
      instance = new IOService();
    }
    return instance;
  }

  public void addTask(IOTask ioTask) throws IOException {
    registerPin(ioTask.getPin());
    ioTasks.add(ioTask);
  }

  @Override
  public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
    return looper;
  }

  public void removeTask(IOTask task) {
    if (task.getPin() != -1) {
      usedPins[task.getPin()] = false;
    }
    ioTasks.remove(task);
  }

  public void start() {
    connectionController.start();
  }

  private void registerPin(int pin) throws IOException {
    if (pin != -1) {
      if (pin < 0 || pin > MAX_PINS) {
        throw new IOException("invalid pin: " + pin);
      }
      if (usedPins[pin] != null && usedPins[pin]) {
        throw new IOException("pin already used: " + pin);
      }
      usedPins[pin] = true;
    }
  }

  public class IOServiceLooper implements IOIOLooper {
    private IOIO ioio;
    private long lastAccessMillis;

    @Override
    public void disconnected() {
      Log.i(TAG, "disconnected");
    }

    @Override
    public void incompatible() {
      Log.i(TAG, "incompatible");
    }

    @Override
    public void incompatible(IOIO ioio) {
      Log.i(TAG, "incompatible");
      ioio.disconnect();
    }

    @Override
    public void loop() throws ConnectionLostException, InterruptedException {
      lastAccessMillis = TimerUtil.tick(lastAccessMillis);
      for (IOTask next: ioTasks) {
        try {
          next.loop();
        } catch (Throwable e) {
          next.setError(e.getLocalizedMessage());
          break;
        }
      }
    }

    @Override
    public void setup(IOIO ioio) {
      this.ioio = ioio;
      this.lastAccessMillis = System.currentTimeMillis();
      for (IOTask next: ioTasks) {
        try {
          next.setup(ioio);
        } catch (Throwable e) {
          next.setError(e.getLocalizedMessage());
          break;
        }
      }
    }
  }
}
