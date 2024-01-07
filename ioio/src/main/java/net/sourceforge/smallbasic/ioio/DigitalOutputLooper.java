package net.sourceforge.smallbasic.ioio;

import net.sourceforge.smallbasic.ioio.AbstractLooper;
import net.sourceforge.smallbasic.ioio.Consumer;

import java.util.concurrent.BlockingQueue;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

class DigitalOutputLooper extends AbstractLooper {
  private static final String TAG = "DigitalOutput";
  private ioio.lib.api.DigitalOutput output;
  private volatile boolean value;

  public DigitalOutputLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
    super(queue, pin);
    value = false;
    Log.i(TAG, "creating DigitalOutputLooper");
  }

  public boolean isReady() {
    return output != null;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    super.loop();
    output.write(value);
    Thread.sleep(10);
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    super.setup(ioio);
    try {
      this.output = ioio.openDigitalOutput(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  void setValue(boolean value) {
    this.value = value;
  }
}
