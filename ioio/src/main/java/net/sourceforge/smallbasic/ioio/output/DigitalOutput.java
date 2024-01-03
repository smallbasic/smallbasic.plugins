package net.sourceforge.smallbasic.ioio.output;

import net.sourceforge.smallbasic.ioio.AbstractLooperProvider;

import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

public class DigitalOutput extends AbstractLooperProvider {
  private static final String TAG = "DigitalOutput";
  private DigitalOutputLooper outputLooper;

  public DigitalOutput() {
    super();
    Log.i(TAG, "created DigitalOutput");
  }

  public void setValue(boolean value) {
    outputLooper.setValue(value);
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return outputLooper;
  }

  public void openOutput(int pin) {
    Log.i(TAG, "openOutput");
    outputLooper = new DigitalOutputLooper(QUEUE, pin);
    start();
  }
}
