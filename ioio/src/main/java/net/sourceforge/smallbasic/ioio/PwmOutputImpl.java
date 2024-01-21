package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.PwmOutput;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class PwmOutputImpl implements PwmOutput, IOTask {
  private static final String TAG = "PulseInput";
  private PwmOutput output;
  private int pin;
  private int freqHz;

  public PwmOutputImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    output.close();
    output = null;
  }


  @Override
  public void loop() throws ConnectionLostException, InterruptedException {

  }

  public void open(int pin, int freqHz) throws IOException {
    Log.i(TAG, "open");
    this.pin = pin;
    IOService.getInstance().addTask(this, pin);
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
