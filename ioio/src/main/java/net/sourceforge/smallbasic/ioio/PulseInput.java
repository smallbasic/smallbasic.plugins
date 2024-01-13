package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class PulseInput extends AbstractLooperProvider {
  private static final String TAG = "PulseInput";
  private static ioio.lib.api.PulseInput.PulseMode pulseMode = ioio.lib.api.PulseInput.PulseMode.NEGATIVE;
  private PulseInputLooper looper;

  public PulseInput() {
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

  public void open(int pin) {
    Log.i(TAG, "open");
    looper = new PulseInputLooper(QUEUE, pin);
    start();
  }

  static class PulseInputLooper extends AbstractLooper {
    private ioio.lib.api.PulseInput input;

    public PulseInputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
      super(queue, pin);
    }

    public void close() {
      this.input.close();
      this.input = null;
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        this.input = ioio.openPulseInput(pin, pulseMode);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }
  }
}
