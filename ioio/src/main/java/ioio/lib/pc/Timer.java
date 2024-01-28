package ioio.lib.pc;

import java.io.IOException;

public class Timer {
  private static final int RATE_MILLIS = 50;

  private Timer() {
    // no access
  }

  public static long tick(long lastAccessMillis) throws InterruptedException {
    long interval = System.currentTimeMillis() - lastAccessMillis;
    if (interval < RATE_MILLIS) {
      Thread.sleep(RATE_MILLIS - interval);
    }
    return System.currentTimeMillis();
  }
}
