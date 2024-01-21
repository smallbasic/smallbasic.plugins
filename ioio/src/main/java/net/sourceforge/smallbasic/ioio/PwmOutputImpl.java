package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

import java.io.IOException;

public class PwmOutputImpl extends IOTask implements PwmOutput {
  private static final String TAG = "PulseInput";
  private PwmOutput output;
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
  public void loop() throws ConnectionLostException, InterruptedException {

  }

  public void open(int pin, int freqHz) throws IOException {
    this.open(pin);
    this.freqHz = freqHz;
  }

  @Override
  public void setDutyCycle(float dutyCycle) throws ConnectionLostException {

  }

  @Override
  public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException {

  }

  @Override
  public void setPulseWidth(float pulseWidthUs) throws ConnectionLostException {

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
