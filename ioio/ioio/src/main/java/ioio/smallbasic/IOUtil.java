package ioio.smallbasic;

import java.util.concurrent.atomic.AtomicReference;

public class IOUtil {
  private static final AtomicReference<String> ERROR = new AtomicReference<>();

  private IOUtil() {
    // no access
  }

  public static String getError() {
    return ERROR.get();
  }

  public static synchronized void setError(String error) {
    if (error != null && ERROR.get() != null && !ERROR.get().contains(error)) {
      ERROR.set(error + " [" + ERROR.get() + "]");
    } else {
      ERROR.set(error);
    }
  }
}
