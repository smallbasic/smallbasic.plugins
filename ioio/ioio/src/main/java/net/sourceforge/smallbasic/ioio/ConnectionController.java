package net.sourceforge.smallbasic.ioio;

import ioio.lib.RuntimeUtil;
import ioio.lib.util.IOIOBaseApplicationHelper;
import ioio.lib.util.IOIOConnectionManager;
import ioio.lib.util.IOIOConnectionRegistry;
import ioio.lib.util.IOIOLooperProvider;

public class ConnectionController extends IOIOBaseApplicationHelper {
  private final IOIOConnectionManager manager = new IOIOConnectionManager(this);

  static {
    if (RuntimeUtil.isRunningOnAndroid()) {
      IOIOConnectionRegistry.addBootstraps(new String[]{
          "ioio.lib.impl.SocketIOIOConnectionBootstrap",
          "ioio.lib.android.accessory.AccessoryConnectionBootstrap",
          "ioio.lib.android.bluetooth.BluetoothIOIOConnectionBootstrap",
          "ioio.lib.android.device.DeviceConnectionBootstrap"});
    } else {
      IOIOConnectionRegistry.addBootstraps(new String[]{
          "ioio.lib.pc.SerialPortIOIOConnectionBootstrap"
      });
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
}
