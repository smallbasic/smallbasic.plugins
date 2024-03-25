package ioio.smallbasic;

import ioio.lib.util.IOIOBaseApplicationHelper;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

public class ConnectionController extends IOIOBaseApplicationHelper {
  private final IOIOConnectionManager manager = new IOIOConnectionManager(this);
  private static final boolean isAndroid;
  private static final String ANDROID_BOOTSTRAP = "ioio.smallbasic.android.AccessoryConnectionBootstrap";
  private static final String PERMISSION_MANAGER = "ioio.smallbasic.android.AccessoryPermissionManager";
  private static final String DESKTOP_BOOTSTRAP = "ioio.smallbasic.pc.SerialPortIOIOConnectionBootstrap";

  static {
    isAndroid = getIsAndroidBuild();
    if (isAndroid) {
      IOIOConnectionRegistry.addBootstraps(new String[] { ANDROID_BOOTSTRAP });
    } else {
      IOIOConnectionRegistry.addBootstraps(new String[] { DESKTOP_BOOTSTRAP });
    }
  }

  public ConnectionController(IOIOLooperProvider provider) {
    super(provider);
  }

  public void start() {
    if (isAndroid) {
      validateAccessoryPermission();
    }
    //manager.start();
  }

  public void stop() {
    manager.stop();
  }

  private static boolean getIsAndroidBuild() {
    boolean result;
    try {
      Class.forName(ANDROID_BOOTSTRAP);
      result = true;
    } catch (ClassNotFoundException e) {
      result = false;
    }
    return result;
  }

  private static void validateAccessoryPermission() {
    try {
      Class.forName(PERMISSION_MANAGER).newInstance();
    }
    catch (Exception e) {
      throw new IOIOException(e.toString());
    }
  }
}
