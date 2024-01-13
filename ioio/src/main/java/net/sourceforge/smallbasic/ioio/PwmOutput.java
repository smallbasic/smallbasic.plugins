package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class PwmOutput extends AbstractLooperProvider {
  private static final String TAG = "PulseInput";
  private PwmOutputLooper looper;

  public PwmOutput() {
    super();
    Log.i(TAG, "created");
  }

  public void close() {
    super.close();
    this.looper.close();
    looper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return looper;
  }

  public void open(int pin, int freqHz) {
    Log.i(TAG, "open");
    looper = new PwmOutputLooper(QUEUE, pin, freqHz);
    start();
  }

  static class PwmOutputLooper extends AbstractLooper {
    private ioio.lib.api.PwmOutput output;
    private final int freqHz;

    public PwmOutputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin, int freqHz) {
      super(queue, pin);
      this.freqHz = freqHz;
    }

    public void close() {
      output.close();
      output = null;
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        output = ioio.openPwmOutput(pin, freqHz);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }
  }
}
