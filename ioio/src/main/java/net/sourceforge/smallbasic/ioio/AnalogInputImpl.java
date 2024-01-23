package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class AnalogInputImpl extends IOTask implements AnalogInput {
  private static final String TAG = "AnalogInput";
  private static final IOLock<AnalogInput> lock = new IOLock<>();
  private AnalogInput input = null;

  public AnalogInputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public int available() throws ConnectionLostException {
    return lock.invokeInt(AnalogInput::available);
  }

  @Override
  public void close() {
    super.close();
    input.close();
    input = null;
  }

  @Override
  public int getOverflowCount() {
    return lock.invokeInt(AnalogInput::getOverflowCount);
  }

  @Override
  public float getReference() {
    return lock.invoke(AnalogInput::getReference);
  }

  @Override
  public float getSampleRate() {
    return lock.invoke(AnalogInput::getSampleRate);
  }

  @Override
  public float getVoltage() {
    return lock.invoke(AnalogInput::getVoltage);
  }

  @Override
  public float getVoltageBuffered() {
    return lock.invoke(AnalogInput::getVoltageBuffered);
  }

  @Override
  public float getVoltageSync() {
    return lock.invoke(AnalogInput::getVoltageSync);
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    lock.process(input);
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return lock.invoke(AnalogInput::read);
  }

  @Override
  public float readBuffered() {
    return lock.invoke(AnalogInput::readBuffered);
  }

  @Override
  public float readSync() {
    return lock.invoke(AnalogInput::readSync);
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
}