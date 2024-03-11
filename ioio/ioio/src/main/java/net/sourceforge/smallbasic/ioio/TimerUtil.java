package net.sourceforge.smallbasic.ioio;

public class TimerUtil {
  private static int latency = 50;

  private TimerUtil() {
    // no access
  }

  public static void setLatency(int latency) {
    TimerUtil.latency = latency;
  }

  public static long tick(long lastAccessMillis) throws InterruptedException {
    long interval = System.currentTimeMillis() - lastAccessMillis;
    if (latency > 0 && interval < latency) {
      Thread.sleep(latency - interval);
    }
    return System.currentTimeMillis();
  }
}
