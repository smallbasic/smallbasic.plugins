package ioio.lib.android;

import android.content.Context;

import java.lang.ref.WeakReference;

import ioio.lib.spi.Log;

public class IOIOLoader {
  private static final String TAG = "IOIOLoader";
  public static native void init(Long app);

  private static WeakReference<Context> context;

  public IOIOLoader(Long activity, Context context) {
    super();
    Log.d(TAG, "IOIOLoader: " + activity);
    IOIOLoader.context = new WeakReference<>(context);
    init(activity);
  }

  public static Context getContext() {
    return context.get();
  }
}
