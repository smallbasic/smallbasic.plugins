package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class CapSense extends AbstractLooperProvider {
  private static final String TAG = "CapSense";
  private CapSenseLooper looper;

  public CapSense() {
    super();
    Log.i(TAG, "created");
  }

  public void close() {
    super.close();
    this.looper.close();
    looper = null;
  }

  @Override
  public IOIOLooper createIOIOLooper(String type, Object extra) {
    return looper;
  }

  public void open(int pin) {
    Log.i(TAG, "open");
    looper = new CapSenseLooper(QUEUE, pin);
    start();
  }

  static class CapSenseLooper extends AbstractLooper {
    private ioio.lib.api.CapSense capSense;

    public CapSenseLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
      super(queue, pin);
    }

    public void close() {
      this.capSense.close();
      this.capSense = null;
    }

    @Override
    public void setup(IOIO ioio) {
      Log.i(TAG, "setup entered");
      super.setup(ioio);
      try {
        this.capSense = ioio.openCapSense(pin);
      }
      catch (ConnectionLostException e) {
        throw new RuntimeException(e);
      }
    }
  }
}

