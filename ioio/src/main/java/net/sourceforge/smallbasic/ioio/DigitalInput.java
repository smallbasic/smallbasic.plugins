package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class DigitalInput extends AbstractLooperProvider implements ioio.lib.api.DigitalInput {
  private static final String TAG = "DigitalInput";
  private DigitalInputLooper looper;

  public DigitalInput() {
    super();
    Log.i(TAG, "created DigitalInput");
  }

  @Override
  public void close() {
    super.close();
    this.looper.close();
    looper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return looper;
  }

  public void open(int pin) {
    Log.i(TAG, "open");
    looper = new DigitalInputLooper(QUEUE, pin);
    start();
  }

  @Override
  public boolean read() {
    return looper.getValue();
  }

  @Override
  public void waitForValue(boolean value) throws InterruptedException, ConnectionLostException {

  }

  static class DigitalInputLooper extends AbstractLooper {
    private ioio.lib.api.DigitalInput input;
    private volatile boolean value;

    public DigitalInputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
      super(queue, pin);
      value = false;
      Log.i(TAG, "creating DigitalInputLooper");
    }

    public void close() {
      this.input.close();
      this.input = null;
    }

    @Override
    public void loop() throws InterruptedException, ConnectionLostException {
      super.loop();
      value = input.read();
      Thread.sleep(10);
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        this.input = ioio.openDigitalInput(pin);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }

    boolean getValue() {
      return this.value;
    }
  }
}
