package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public abstract class AbstractLooper implements IOIOLooper {
  static final String TAG = "AbstractLooper";
  protected IOIO ioio;
  protected boolean isIncompatible;
  protected int pin;
  private final BlockingQueue<Consumer<IOIO>> queue;

  public AbstractLooper(BlockingQueue<Consumer<IOIO>> queue, int pin) {
    this.queue = queue;
    this.pin = pin;
    this.ioio = null;
    this.isIncompatible = false;
  }

  @Override
  public void disconnected() {
    Log.i(TAG, "disconnected");
  }

  @Override
  public void incompatible() {
    Log.i(TAG, "incompatible");
    isIncompatible = true;
  }

  @Override
  public void incompatible(IOIO ioio) {
    Log.i(TAG, "incompatible");
    this.isIncompatible = true;
    ioio.disconnect();
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    if (!queue.isEmpty()) {
      try {
        queue.take().invoke(ioio);
      }
      catch (ConnectionLostException | IncompatibilityException e) {
        throw new RuntimeException(e);
      }
    }
  }

  @Override
  public void setup(IOIO ioio) {
    this.ioio = ioio;
    this.isIncompatible = false;
  }
}
