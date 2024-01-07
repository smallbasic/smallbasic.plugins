package net.sourceforge.smallbasic.ioio;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

import ioio.lib.api.IOIO;
import ioio.lib.spi.Log;
import ioio.lib.util.IOIOLooperProvider;

public abstract class AbstractLooperProvider implements IOIOLooperProvider {
  static final protected String TAG = "AbstractLooperProvider";
  static final protected BlockingQueue<Consumer<IOIO>> QUEUE = new LinkedBlockingQueue<>();
  final protected ConnectionController controller;
  protected boolean ready;

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
    final CountDownLatch latch = new CountDownLatch(1);
    try {
      if (this.ready) {
        QUEUE.put(ioio -> {
          consumer.invoke(ioio);
          latch.countDown();
        });
        latch.await();
      } else {
        Log.e(TAG, "Connection not ready");
      }
    } catch (InterruptedException e) {
      Log.e(TAG, "Error putting message handler to the queue: ", e);
    }
  }
}
