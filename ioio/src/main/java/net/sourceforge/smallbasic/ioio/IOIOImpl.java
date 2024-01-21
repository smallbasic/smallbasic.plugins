package net.sourceforge.smallbasic.ioio;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.spi.Log;

public class IOIOImpl implements IOTask {
  private static final String TAG = "IOIOImpl";
  private final BlockingQueue<Consumer<IOIO>> queue = new LinkedBlockingQueue<>();
  private IOIO ioio;

  public IOIOImpl() {
    Log.i(TAG, "created");
  }

  public void beginBatch() {
    invoke(IOIO::beginBatch);
  }

  public void disconnect() {
    invoke(IOIO::disconnect);
  }

  public void endBatch() {
    invoke(IOIO::endBatch);
  }

  public void hardReset() {
    invoke(IOIO::hardReset);
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    if (!queue.isEmpty()) {
      System.out.println("queue has item");
      try {
        queue.take().invoke(ioio);
        System.out.println("invoked");
      }
      catch (ConnectionLostException | IncompatibilityException e) {
        throw new RuntimeException(e);
      }
    }
  }

  @Override
  public void setup(IOIO ioio) {
    this.ioio = ioio;
  }

  public void softReset() {
    invoke(IOIO::softReset);
  }

  public void sync() {
    invoke(IOIO::sync);
  }

  public void waitForConnect() {
    invoke(IOIO::waitForConnect);
  }

  public void waitForDisconnect() {
    invoke(IOIO::waitForDisconnect);
  }

  protected void invoke(Consumer<IOIO> consumer) {
    final CountDownLatch latch = new CountDownLatch(1);
    try {
      queue.put(ioio -> {
          consumer.invoke(ioio);
          latch.countDown();
        });
      latch.await();
    } catch (InterruptedException e) {
      Log.e(TAG, "Error putting message handler to the queue: ", e);
    }
  }
}
