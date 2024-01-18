package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class DigitalOutputImpl extends AbstractLooperProvider implements DigitalOutput {
  private static final String TAG = "DigitalOutput";
  private DigitalOutputLooper looper;

  public DigitalOutputImpl() {
    super();
    Log.i(TAG, "created DigitalOutput");
  }

  public void close() {
    super.close();
    looper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return looper;
  }

  public void open(int pin) {
    Log.i(TAG, "open");
    looper = new DigitalOutputLooper(QUEUE, pin);
    start();
  }

  @Override
  public void write(boolean val) throws ConnectionLostException {
    looper.setValue(val);
  }

  static class DigitalOutputLooper extends AbstractLooper {
    private DigitalOutput output;
    private volatile boolean value;

    public DigitalOutputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
      super(queue, pin);
      value = false;
      Log.i(TAG, "creating DigitalOutputLooper");
    }

    public void close() {
      output.close();
      output = null;
    }

    @Override
    public void loop() throws InterruptedException, ConnectionLostException {
      super.loop();
      output.write(value);
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        this.output = ioio.openDigitalOutput(pin);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }

    void setValue(boolean value) {
      this.value = value;
    }
  }
}
