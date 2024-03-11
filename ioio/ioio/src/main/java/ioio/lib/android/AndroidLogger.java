package ioio.lib.android;

import android.util.Log;

public class AndroidLogger implements ioio.lib.spi.Log.ILogger {
  public AndroidLogger() {
    super();
  }

  @Override
  public void write(int level, String tag, String message) {
    Log.println(level, tag, message);
  }
}
