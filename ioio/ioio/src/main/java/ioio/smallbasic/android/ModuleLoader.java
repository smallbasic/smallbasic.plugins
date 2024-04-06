package ioio.smallbasic.android;

import android.content.Context;

import java.lang.ref.WeakReference;

import ioio.lib.spi.Log;

/**
 * ModuleLoader - Invoked from the "app" to commence loading
 */
public class ModuleLoader {
  private static final String TAG = "ModuleLoader";
  public static native void init(Long app);

  private static WeakReference<Context> context;

  public ModuleLoader(Long activity, Context context) {
    super();
    Log.d(TAG, "ModuleLoader: " + activity);
    ModuleLoader.context = new WeakReference<>(context);
    init(activity);
  }

  public static Context getContext() {
    return context.get();
  }
}
