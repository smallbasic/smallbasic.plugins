package net.sourceforge.smallbasic.ioio.input;

import net.sourceforge.smallbasic.ioio.AbstractLooper;
import net.sourceforge.smallbasic.ioio.Consumer;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

import java.util.concurrent.BlockingQueue;

public class AnalogInputLooper extends AbstractLooper {
  private static final String TAG = "AnalogInput";
  private AnalogInput analogInput;

  public AnalogInputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
    super(queue, pin);
    Log.i(TAG, "creating AnalogInputLooper");
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
