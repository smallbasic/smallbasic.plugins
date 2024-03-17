package ioio.lib;

public class AndroidUtil {
  private static final boolean isRunningOnAndroid;
  static {
    isRunningOnAndroid = getIsRunningOnAndroid();
  }

  public static boolean isAndroid() {
    return isRunningOnAndroid;
  }

  private static boolean getIsRunningOnAndroid() {
    boolean result;
    try {
      Class.forName("android.os.Build");
      result = true;
    } catch (ClassNotFoundException e) {
      result = false;
    }
    return result;
  }
}
