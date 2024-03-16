package net.sourceforge.smallbasic.ioio;

import ioio.lib.spi.Log;

public class IOIOLoader {
  private static final String TAG = "IOIOLoader";
  public static native void init(Class<?> clazz);

  public IOIOLoader() {
    super();
    init(getClass());
  }

  public static Class<?> findClass(String className) {
    try {
      Log.i(TAG, "findClass " + className);
      return Class.forName("net.sourceforge.smallbasic.ioio." + className);
    }
    catch (ClassNotFoundException e) {
      throw new RuntimeException(e);
    }
  }
}
