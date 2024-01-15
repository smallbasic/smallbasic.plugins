package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class PulseInput extends AbstractLooperProvider implements ioio.lib.api.PulseInput {
  private static final String TAG = "PulseInput";
  private static final ioio.lib.api.PulseInput.PulseMode pulseMode = ioio.lib.api.PulseInput.PulseMode.NEGATIVE;
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

  @Override
  public float getDuration() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float getDurationSync() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float getDurationBuffered() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float waitPulseGetDuration() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float getFrequency() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float getFrequencySync() throws InterruptedException, ConnectionLostException {
    return 0;
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
