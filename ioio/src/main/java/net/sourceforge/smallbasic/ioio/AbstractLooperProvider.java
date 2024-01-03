package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooperProvider;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public abstract class AbstractLooperProvider implements IOIOLooperProvider {
  static final String TAG = "AbstractLooperProvider";
  static final BlockingQueue<Consumer<IOIO>> QUEUE = new LinkedBlockingQueue<>();
  final ConnectionController controller;
  boolean ready;

  protected AbstractLooperProvider() {
    this.controller = new ConnectionController(this);
    this.ready = false;
  }

  public void beginBatch() {
    putMethod(IOIO::beginBatch);
  }

  public void disconnect() {
    putMethod(IOIO::disconnect);
  }

  public void endBatch() {
    putMethod(IOIO::endBatch);
  }

  public void hardReset() {
    putMethod(IOIO::hardReset);
  }

  public void softReset() {
    putMethod(IOIO::softReset);
  }

  public void stop() {
    this.controller.stop();
    this.ready = false;
  }

  public void sync() {
    putMethod(IOIO::sync);
  }

  public void waitForConnect() {
    putMethod(IOIO::waitForConnect);
  }

  public void waitForDisconnect() {
    putMethod(IOIO::waitForDisconnect);
  }

  protected void start() {
    this.controller.start();
    this.ready = true;
  }

  private void putMethod(Consumer<IOIO> consumer) {
    try {
      if (this.ready) {
        QUEUE.put(consumer);
      } else {
        Log.e(TAG, "Connection not ready");
      }
    } catch (InterruptedException e) {
      Log.e(TAG, "Error putting message handler to the queue: ", e);
    }
  }
}
