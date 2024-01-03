package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooper;

import java.util.concurrent.BlockingQueue;

public abstract class AbstractLooper implements IOIOLooper {
  static final String TAG = "AbstractLooper";
  final BlockingQueue<Consumer<IOIO>> messageQueue;
  final String connectionType;
  final Object extra;
  IOIO ioio;
  boolean isIncompatible;
  int pin;

  public AbstractLooper(BlockingQueue<Consumer<IOIO>> messageQueue, String connectionType, Object extra, int pin) {
    this.messageQueue = messageQueue;
    this.connectionType = connectionType;
    this.extra = extra;
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
  public void loop() throws InterruptedException {
    Log.i(TAG, "loop entered");
    if (!messageQueue.isEmpty()) {
      try {
        messageQueue.take().invoke(ioio);
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
