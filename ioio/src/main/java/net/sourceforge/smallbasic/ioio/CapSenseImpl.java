package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.CapSense;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public class CapSenseImpl extends AbstractLooperProvider implements CapSense {
  private static final String TAG = "CapSense";
  private CapSenseLooper looper;

  public CapSenseImpl() {
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

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float readSync() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public void setFilterCoef(float t) throws ConnectionLostException {

  }

  @Override
  public void waitOver(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitOverSync(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitUnder(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitUnderSync(float threshold) throws ConnectionLostException, InterruptedException {

  }

  static class CapSenseLooper extends AbstractLooper {
    private CapSense capSense;

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

