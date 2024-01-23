package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class IOLock<I> {
  private final ReadWriteLock lock = new ReentrantReadWriteLock();
  private Consumer<I> consumer;

  public float invoke(Function<Float, I> function) {
    CountDownLatch latch = beginLatch();
    lock.writeLock().lock();
    AtomicReference<Float> result = new AtomicReference<>();
    try {
      this.consumer = (i) -> {
        result.set(function.apply(i));
        latch.countDown();
      };
    }
    finally {
      lock.writeLock().unlock();
    }
    endLatch(latch);
    return result.get();
  }

  public void invoke(Consumer<I> consumer) {
    CountDownLatch latch = beginLatch();
    lock.writeLock().lock();
    try {
      this.consumer = (i) -> {
        consumer.accept(i);
        latch.countDown();
      };
    }
    finally {
      lock.writeLock().unlock();
    }
    endLatch(latch);
  }

  public int invokeInt(Function<Integer, I> function) {
    CountDownLatch latch = beginLatch();
    lock.writeLock().lock();
    AtomicReference<Integer> result = new AtomicReference<>();
    try {
      this.consumer = (i) -> {
        result.set(function.apply(i));
        latch.countDown();
      };
    }
    finally {
      lock.writeLock().unlock();
    }
    endLatch(latch);
    return result.get();
  }

  public void process(I input) {
    lock.readLock().lock();
    try {
      if (input != null && consumer != null) {
        consumer.accept(input);
        consumer = null;
      }
    }
    catch (IncompatibilityException | ConnectionLostException | InterruptedException e) {
      throw new RuntimeException(e);
    }
    finally {
      lock.readLock().unlock();
    }
  }

  /**
   * CountDownLatch ensures the calling method returns once process has completed
   */
  private CountDownLatch beginLatch() {
    return new CountDownLatch(1);
  }

  /**
   * Wait for the looper to process the next consumer
   */
  private void endLatch(CountDownLatch latch) {
    try {
      latch.await();
    } catch (InterruptedException e) {
      throw new RuntimeException(e);
    }
  }
}
