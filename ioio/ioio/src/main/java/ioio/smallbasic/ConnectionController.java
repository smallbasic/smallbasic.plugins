package ioio.smallbasic;

import ioio.lib.util.IOIOBaseApplicationHelper;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

public class ConnectionController extends IOIOBaseApplicationHelper {
  private final IOIOConnectionManager manager = new IOIOConnectionManager(this);
  private static final boolean isAndroid;
  private static final String ACCESSORY_BOOTSTRAP = "ioio.smallbasic.android.AccessoryConnectionBootstrap";
  private static final String SERIAL_PORT_BOOTSTRAP = "ioio.smallbasic.pc.SerialPortIOIOConnectionBootstrap";
  private static final String PERMISSION_CHECK = "ioio.smallbasic.android.AccessoryPermissionCheck";

  static {
    isAndroid = getIsAndroidBuild();
    if (isAndroid) {
      IOIOConnectionRegistry.addBootstraps(new String[] { ACCESSORY_BOOTSTRAP });
    } else {
      IOIOConnectionRegistry.addBootstraps(new String[] { SERIAL_PORT_BOOTSTRAP });
    }
  }

  public ConnectionController(IOIOLooperProvider provider) {
    super(provider);
  }

  public void start() {
    if (isAndroid) {
      permitAccessory();
    }
    manager.start();
  }

  public void stop() {
    manager.stop();
  }

  private static boolean getIsAndroidBuild() {
    boolean result;
    try {
      Class.forName(ACCESSORY_BOOTSTRAP);
      result = true;
    } catch (ClassNotFoundException e) {
      result = false;
    }
    return result;
  }

  private static void permitAccessory() {
    try {
      Class.forName(PERMISSION_CHECK).newInstance();
    }
    catch (Exception e) {
      throw new IOIOException(e);
    }
  }
}
