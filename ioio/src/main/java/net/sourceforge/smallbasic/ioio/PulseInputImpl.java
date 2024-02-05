package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.PulseInput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class PulseInputImpl extends IOTask implements PulseInput {
  private static final String TAG = "PulseInput";
  private static final PulseInput.PulseMode pulseMode = PulseInput.PulseMode.NEGATIVE;
  private final IOLock<PulseInput> lock = new IOLock<>();
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
  public float getDuration() {
    return lock.invoke(PulseInput::getDuration);
  }

  @Override
  public float getDurationBuffered() {
    return lock.invoke(PulseInput::getDurationBuffered);
  }

  @Override
  public float getDurationSync() {
    return lock.invoke(PulseInput::getDurationSync);
  }

  @Override
  public float getFrequency() {
    return lock.invoke(PulseInput::getFrequency);
  }

  @Override
  public float getFrequencySync() {
    return lock.invoke(PulseInput::getFrequencySync);
  }

  @Override
  public void loop() {
    lock.process(input);
  }

  @Override
  public void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    input = ioio.openPulseInput(pin, pulseMode);
  }

  @Override
  public float waitPulseGetDuration() {
    return lock.invoke(PulseInput::getDurationBuffered);
  }
}
