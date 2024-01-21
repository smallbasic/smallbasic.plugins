package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;

public class AnalogInputImpl implements AnalogInput, IOTask {
  private static final String TAG = "AnalogInput";
  private final BlockingQueue<FloatConsumer<AnalogInput>> queue = new LinkedBlockingQueue<>();
  private AnalogInput input;
  private int pin;

  public AnalogInputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public int available() throws ConnectionLostException {
    return 0;
  }

  @Override
  public void close() {
    input.close();
    input = null;
  }

  @Override
  public int getOverflowCount() {
    return (int) invoke(AnalogInput::getOverflowCount);
  }

  @Override
  public float getReference() {
    return invoke(AnalogInput::getReference);
  }

  @Override
  public float getSampleRate() {
    return invoke(AnalogInput::getSampleRate);
  }

  @Override
  public float getVoltage() {
    return invoke(AnalogInput::getVoltage);
  }

  @Override
  public float getVoltageBuffered() {
    return invoke(AnalogInput::getVoltageBuffered);
  }

  @Override
  public float getVoltageSync() {
    return invoke(AnalogInput::getVoltageSync);
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
    Log.i(TAG, "openInput");
    this.pin = pin;
    IOService.getInstance().addTask(this, pin);
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return invoke(AnalogInput::read);
  }

  @Override
  public float readBuffered() {
    return invoke(AnalogInput::readBuffered);
  }

  @Override
  public float readSync() {
    return invoke(AnalogInput::readSync);
  }

  @Override
  public void setBuffer(int capacity) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      input = ioio.openAnalogInput(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  protected float invoke(FloatConsumer<AnalogInput> consumer) {
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
