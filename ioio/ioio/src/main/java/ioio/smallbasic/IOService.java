package ioio.smallbasic;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.IOIOLooperProvider;

public class IOService implements IOIOLooperProvider  {
  private static final String TAG = "IOService";
  private static final int MAX_PINS = 46;
  private static final AtomicBoolean HARD_RESET = new AtomicBoolean(false);
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

  public static boolean getHardReset() {
    return HARD_RESET.get();
  }

  public static IOService getInstance() {
    if (instance == null) {
      instance = new IOService();
    }
    return instance;
  }

  public static void setHardReset(boolean hardReset) {
    IOService.HARD_RESET.set(hardReset);
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

  public void stop() {
    if (looper.ioio != null) {
      looper.ioio.disconnect();
    }
    connectionController.stop();
  }

  private void registerPin(int pin) throws IOException {
    if (pin != -1) {
      if (pin < 0 || pin > MAX_PINS) {
        throw new IOException("Invalid pin: " + pin);
      }
      if (usedPins[pin] != null && usedPins[pin]) {
        throw new IOException("Pin already used: " + pin);
      }
      usedPins[pin] = true;
    }
  }

  public class IOServiceLooper implements IOIOLooper {
    private IOIO ioio;

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
      Thread.sleep(0, 5);
      for (IOTask next: ioTasks) {
        try {
          next.loop();
        } catch (Throwable e) {
          IOUtil.setError(e.getLocalizedMessage());
          break;
        }
      }
    }

    @Override
    public void setup(IOIO ioio) {
      this.ioio = ioio;
      for (IOTask next: ioTasks) {
        try {
          next.setup(ioio);
        } catch (Throwable e) {
          IOUtil.setError(e.getLocalizedMessage());
          break;
        }
      }
    }
  }
}
