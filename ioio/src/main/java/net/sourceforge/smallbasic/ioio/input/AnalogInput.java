package net.sourceforge.smallbasic.ioio.input;

import net.sourceforge.smallbasic.ioio.AbstractLooperProvider;

import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

public class AnalogInput extends AbstractLooperProvider {
  private static final String TAG = "AnalogInput";
  private AnalogInputLooper looper;

  public AnalogInput() {
    super();
    Log.i(TAG, "created AnalogInput");
  }

  @Override
  public IOIOLooper createIOIOLooper(String connectionType, Object extra) {
    return looper;
  }

  public void openInput(int pin) {
    Log.i(TAG, "openInput");
    looper = new AnalogInputLooper(QUEUE, pin);
    start();
  }
}
