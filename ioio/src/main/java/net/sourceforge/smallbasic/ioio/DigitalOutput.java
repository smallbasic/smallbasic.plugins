package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class DigitalOutput extends AbstractLooperProvider implements ioio.lib.api.DigitalInput {
  private static final String TAG = "DigitalOutput";
  private DigitalOutputLooper outputLooper;

  public DigitalOutput() {
    super();
    Log.i(TAG, "created DigitalOutput");
  }

  public void close() {
    super.close();
    outputLooper.close();
    outputLooper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return outputLooper;
  }

  public void open(int pin) {
    Log.i(TAG, "open");
    outputLooper = new DigitalOutputLooper(QUEUE, pin);
    start();
  }

  @Override
  public boolean read() throws InterruptedException, ConnectionLostException {
    return false;
  }

  @Override
  public void waitForValue(boolean value) throws InterruptedException, ConnectionLostException {

  }

  public void write(int value) {
    outputLooper.setValue(value == 1);
  }

  static class DigitalOutputLooper extends AbstractLooper {
    private ioio.lib.api.DigitalOutput output;
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
      Thread.sleep(10);
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
