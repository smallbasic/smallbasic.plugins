package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

public class AnalogInput extends AbstractLooperProvider implements ioio.lib.api.AnalogInput {
  private static final String TAG = "AnalogInput";
  private AnalogInputLooper looper;
  static final protected BlockingQueue<FloatConsumer<ioio.lib.api.AnalogInput>> INPUT_QUEUE = new LinkedBlockingQueue<>();

  public AnalogInput() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public int available() throws ConnectionLostException {
    return 0;
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

  @Override
  public int getOverflowCount() {
    return (int) invokeFloat(ioio.lib.api.AnalogInput::getOverflowCount);
  }

  @Override
  public float getReference() {
    return invokeFloat(ioio.lib.api.AnalogInput::getReference);
  }

  @Override
  public float getSampleRate() {
    return invokeFloat(ioio.lib.api.AnalogInput::getSampleRate);
  }

  @Override
  public float getVoltage() {
    return invokeFloat(ioio.lib.api.AnalogInput::getVoltage);
  }

  @Override
  public float getVoltageBuffered() {
    return invokeFloat(ioio.lib.api.AnalogInput::getVoltageBuffered);
  }

  @Override
  public float getVoltageSync() {
    return invokeFloat(ioio.lib.api.AnalogInput::getVoltageSync);
  }

  public void open(int pin) {
    Log.i(TAG, "openInput");
    looper = new AnalogInputLooper(QUEUE, pin);
    start();
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return invokeFloat(ioio.lib.api.AnalogInput::read);
  }

  @Override
  public float readBuffered() {
    return invokeFloat(ioio.lib.api.AnalogInput::readBuffered);
  }

  @Override
  public float readSync() {
    return invokeFloat(ioio.lib.api.AnalogInput::readSync);
  }

  @Override
  public void setBuffer(int capacity) {

  }

  protected float invokeFloat(FloatConsumer<ioio.lib.api.AnalogInput> consumer) {
    final CountDownLatch latch = new CountDownLatch(1);
    final float[] result = new float[1];
    try {
      INPUT_QUEUE.put(e -> {
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
    public void loop() throws InterruptedException, ConnectionLostException {
      super.loop();
      if (!INPUT_QUEUE.isEmpty()) {
        try {
          INPUT_QUEUE.take().invoke(analogInput);
        }
        catch (ConnectionLostException | IncompatibilityException e) {
          throw new RuntimeException(e);
        }
      }
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
