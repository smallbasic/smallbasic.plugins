package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.PulseInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class PulseInputImpl extends IOTask implements PulseInput {
  private static final String TAG = "PulseInput";
  private static final PulseInput.PulseMode pulseMode = PulseInput.PulseMode.NEGATIVE;
  private static final IOLock<PulseInput> lock = new IOLock<>();
  private PulseInput input;

  public PulseInputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    input.close();
    input = null;
  }

  @Override
  public float getDuration() throws InterruptedException, ConnectionLostException {
    return lock.invoke(PulseInput::getDuration);
  }

  @Override
  public float getDurationBuffered() throws InterruptedException, ConnectionLostException {
    return lock.invoke(PulseInput::getDurationBuffered);
  }

  @Override
  public float getDurationSync() throws InterruptedException, ConnectionLostException {
    return lock.invoke(PulseInput::getDurationSync);
  }

  @Override
  public float getFrequency() throws InterruptedException, ConnectionLostException {
    return lock.invoke(PulseInput::getFrequency);
  }

  @Override
  public float getFrequencySync() throws InterruptedException, ConnectionLostException {
    return lock.invoke(PulseInput::getFrequencySync);
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    lock.process(input);
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
}
