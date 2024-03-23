package net.sourceforge.smallbasic.ioio;

import ioio.lib.util.IOIOBaseApplicationHelper;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

public class ConnectionController extends IOIOBaseApplicationHelper {
  private final IOIOConnectionManager manager = new IOIOConnectionManager(this);

  static {
    if (getIsRunningOnAndroid()) {
      IOIOConnectionRegistry.addBootstraps(new String[]{"ioio.lib.android.AccessoryConnectionBootstrap"});
    } else {
      IOIOConnectionRegistry.addBootstraps(new String[]{"ioio.lib.pc.SerialPortIOIOConnectionBootstrap"});
    }
  }

  public ConnectionController(IOIOLooperProvider provider) {
    super(provider);
  }

  public void start() {
    manager.start();
  }

  public void stop() {
    manager.stop();
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
