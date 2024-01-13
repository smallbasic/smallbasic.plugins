package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class AnalogInput extends AbstractLooperProvider {
  private static final String TAG = "AnalogInput";
  private AnalogInputLooper looper;

  public AnalogInput() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    this.looper.close();
    this.looper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
    return looper;
  }

  public void open(int pin) {
    Log.i(TAG, "openInput");
    looper = new AnalogInputLooper(QUEUE, pin);
    start();
  }

  static class AnalogInputLooper extends AbstractLooper {
    private ioio.lib.api.AnalogInput analogInput;

    public AnalogInputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
      super(queue, pin);
      Log.i(TAG, "creating AnalogInputLooper");
    }

    public void close() {
      this.analogInput.close();
      this.analogInput = null;
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        analogInput = ioio.openAnalogInput(pin);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }
  }
}
