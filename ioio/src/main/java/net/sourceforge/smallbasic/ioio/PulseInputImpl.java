package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

import ioio.lib.api.IOIO;
import ioio.lib.api.PulseInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;

public class PulseInputImpl implements PulseInput, IOTask {
  private static final String TAG = "PulseInput";
  private static final PulseInput.PulseMode pulseMode = PulseInput.PulseMode.NEGATIVE;
  private final BlockingQueue<FloatConsumer<PulseInput>> queue = new LinkedBlockingQueue<>();
  private int pin;
  private PulseInput input;

  public PulseInputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    IOService.getInstance().removeTask(this);
    input.close();
    input = null;
  }

  @Override
  public float getDuration() throws InterruptedException, ConnectionLostException {
    return invoke(PulseInput::getDuration);
  }

  @Override
  public float getDurationBuffered() throws InterruptedException, ConnectionLostException {
    return invoke(PulseInput::getDurationBuffered);
  }

  @Override
  public float getDurationSync() throws InterruptedException, ConnectionLostException {
    return invoke(PulseInput::getDurationSync);
  }

  @Override
  public float getFrequency() throws InterruptedException, ConnectionLostException {
    return invoke(PulseInput::getFrequency);
  }

  @Override
  public float getFrequencySync() throws InterruptedException, ConnectionLostException {
    return invoke(PulseInput::getFrequencySync);
  }

  @Override
  public int getPin() {
    return pin;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    if (!queue.isEmpty()) {
      try {
        queue.take().invoke(input);
      }
      catch (ConnectionLostException | IncompatibilityException e) {
        throw new RuntimeException(e);
      }
    }
  }

  public void open(int pin) throws IOException {
    Log.i(TAG, "open");
    this.pin = pin;
    IOService.getInstance().addTask(this);
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      input = ioio.openPulseInput(pin, pulseMode);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  public float waitPulseGetDuration() throws InterruptedException, ConnectionLostException {
    throw new UnsupportedOperationException();
  }

  protected float invoke(FloatConsumer<PulseInput> consumer) {
    CountDownLatch latch = new CountDownLatch(1);
    float[] result = new float[1];
    try {
      queue.put(e -> {
          result[0] = consumer.invoke(e);
          latch.countDown();
          return result[0];
        });
      latch.await();
    }
    catch (InterruptedException e) {
      throw new RuntimeException(e);
    }
    return result[0];
  }
}
