package net.sourceforge.smallbasic.ioio;

import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

public class AnalogInput extends AbstractLooperProvider {
  private static final String TAG = "AnalogController";
  private int pin;

  public AnalogInput() {
    super();
    Log.i(TAG, "created AnalogController");
  }

  @Override
  public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
    return new AnalogInputLooper(QUEUE, connectionType, extra, pin);
  }

  public void openInput(int pin) {
    Log.i(TAG, "openInput");
    this.pin = pin;
    start();
  }
}
