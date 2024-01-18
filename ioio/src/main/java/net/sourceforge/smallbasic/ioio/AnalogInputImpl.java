package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;
import ioio.lib.api.AnalogInput;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

public class AnalogInputImpl extends AbstractLooperProvider implements AnalogInput {
  private static final String TAG = "AnalogInput";
  private AnalogInputLooper looper;
  protected BlockingQueue<FloatConsumer<AnalogInput>> inputQueue;

  public AnalogInputImpl() {
    super();
    inputQueue = new LinkedBlockingQueue<>();
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
    return (int) invokeFloat(AnalogInput::getOverflowCount);
  }

  @Override
  public float getReference() {
    return invokeFloat(AnalogInput::getReference);
  }

  @Override
  public float getSampleRate() {
    return invokeFloat(AnalogInput::getSampleRate);
  }

  @Override
  public float getVoltage() {
    return invokeFloat(AnalogInput::getVoltage);
  }

  @Override
  public float getVoltageBuffered() {
    return invokeFloat(AnalogInput::getVoltageBuffered);
  }

  @Override
  public float getVoltageSync() {
    return invokeFloat(AnalogInput::getVoltageSync);
  }

  public void open(int pin) {
    Log.i(TAG, "openInput");
    looper = new AnalogInputLooper(QUEUE, inputQueue, pin);
    start();
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return invokeFloat(AnalogInput::read);
  }

  @Override
  public float readBuffered() {
    return invokeFloat(AnalogInput::readBuffered);
  }

  @Override
  public float readSync() {
    return invokeFloat(AnalogInput::readSync);
  }

  @Override
  public void setBuffer(int capacity) {

  }

  protected float invokeFloat(FloatConsumer<AnalogInput> consumer) {
    final CountDownLatch latch = new CountDownLatch(1);
    final float[] result = new float[1];
    try {
      inputQueue.put(e -> {
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
    private AnalogInput analogInput;
    private final BlockingQueue<FloatConsumer<AnalogInput>> inputQueue;

    public AnalogInputLooper(BlockingQueue<Consumer<IOIO>> queue,
                             BlockingQueue<FloatConsumer<AnalogInput>> inputQueue,
                             int pin) {
      super(queue, pin);
      this.inputQueue = inputQueue;
      Log.i(TAG, "creating AnalogInputLooper");
    }

    public void close() {
      this.analogInput.close();
      this.analogInput = null;
    }

    @Override
    public void loop() throws InterruptedException, ConnectionLostException {
      super.loop();
      if (!inputQueue.isEmpty()) {
        try {
          inputQueue.take().invoke(analogInput);
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
