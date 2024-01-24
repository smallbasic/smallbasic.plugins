package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicReference;

import ioio.lib.api.IOIO;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class PwmOutputImpl extends IOTask implements PwmOutput {
  private static final String TAG = "PulseInput";
  private static final float UNSET = -1f;
  private PwmOutput output;
  private final AtomicReference<Float> dutyCycle = new AtomicReference<>(UNSET);
  private final AtomicReference<Float> pulseWidth = new AtomicReference<>(UNSET);
  private int freqHz;

  public PwmOutputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    output.close();
    output = null;
  }

  @Override
  public void loop() throws ConnectionLostException {
    if (dutyCycle.get() != UNSET) {
      output.setDutyCycle(dutyCycle.get());
    }
    if (pulseWidth.get() != UNSET) {
      output.setPulseWidth(pulseWidth.get());
    }
  }

  public void open(int pin, int freqHz) throws IOException {
    this.open(pin);
    this.freqHz = freqHz;
  }

  @Override
  public void setDutyCycle(float dutyCycle) {
    this.dutyCycle.set(dutyCycle);
  }

  @Override
  public void setPulseWidth(int pulseWidthUs) {
    this.pulseWidth.set((float) pulseWidthUs);
  }

  @Override
  public void setPulseWidth(float pulseWidthUs) {
    this.pulseWidth.set(pulseWidthUs);
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      output = ioio.openPwmOutput(pin, freqHz);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }
}
