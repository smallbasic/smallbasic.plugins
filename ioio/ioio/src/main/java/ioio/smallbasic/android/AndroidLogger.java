package ioio.smallbasic.android;

import android.annotation.TargetApi;
import android.os.Build;
import android.util.Log;

public class AndroidLogger implements ioio.lib.spi.Log.ILogger {
  public AndroidLogger() {
    super();
  }

  @TargetApi(Build.VERSION_CODES.O)
  @Override
  public void write(int level, String tag, String message) {
    long id = Thread.currentThread().getId();
    String text = tag + ": [#" + id + "] " + message;
    Log.println(Log.ERROR, "smallbasic", text);
  }
}
