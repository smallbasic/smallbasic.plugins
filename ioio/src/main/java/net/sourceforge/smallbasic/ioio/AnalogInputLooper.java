package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

import java.util.concurrent.BlockingQueue;

public class AnalogInputLooper extends AbstractLooper {
  private AnalogInput analogInput;

  public AnalogInputLooper(BlockingQueue<Consumer<IOIO>> messageQueue,
                           String connectionType,
                           Object extra,
                           int pin) {
    super(messageQueue, connectionType, extra, pin);
    Log.i(TAG, "creating AnalogInputLooper");
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    super.setup(ioio);
    try {
      this.analogInput = ioio.openAnalogInput(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }
}
