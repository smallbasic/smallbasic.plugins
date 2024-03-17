package net.sourceforge.smallbasic.ioio;

import ioio.lib.spi.Log;

public class IOIOLoader {
  private static final String TAG = "IOIOLoader";
  public static native void init(Long app);

  public IOIOLoader(Long activity) {
    super();
    Log.d(TAG, "IOIOLoader: " + activity);
    init(activity);
  }
}
