package ioio.smallbasic.android;

import android.annotation.TargetApi;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;

import ioio.lib.spi.Log;
import ioio.smallbasic.IOIOException;

public class AccessoryPermissionCheck extends BroadcastReceiver {
  private static final String TAG = AccessoryPermissionCheck.class.getSimpleName();
  private static final String ACTION_USB_PERMISSION = "ioio.smallbasic.android.USB_PERMISSION";
  private static final String PERMISSION_ERROR = "Not permitted";

  @TargetApi(Build.VERSION_CODES.TIRAMISU)
  public AccessoryPermissionCheck() {
    Log.d(TAG, "AccessoryPermissionCheck entered");
    UsbAccessory accessory = UsbUtil.getUsbAccessory();
    if (accessory == null || !"IOIO".equals(accessory.getModel())) {
      throw new IOIOException("IOIO board not found.");
    }

    UsbManager usbManager = UsbUtil.getUsbManager();
    if (!usbManager.hasPermission(accessory)) {
      new Handler(Looper.getMainLooper()).post(() -> {
        Context context = ModuleLoader.getContext();
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        filter.setPriority(IntentFilter.SYSTEM_HIGH_PRIORITY - 1);
        context.registerReceiver(this, filter, Context.RECEIVER_NOT_EXPORTED);
        int flags = PendingIntent.FLAG_IMMUTABLE;
        Intent intent = new Intent(ACTION_USB_PERMISSION);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
        usbManager.requestPermission(accessory, pendingIntent);
      });
      // for some reason using a latch here causes an ANR
      Log.d(TAG, "requesting permission");
      throw new IOIOException(PERMISSION_ERROR);
    }
  }

  @Override
  public synchronized void onReceive(final Context context, Intent intent) {
    Log.d(TAG, "onReceive entered");
    if (ACTION_USB_PERMISSION.equals(intent.getAction())) {
      UsbAccessory accessory = UsbUtil.getUsbAccessory();
      String version = accessory != null ? accessory.getVersion() : "";
      boolean permitted = UsbUtil.getUsbManager().hasPermission(accessory);
      final String message = "IOIO board [" + version + "] access " + (permitted ? "permitted" : "denied");
      final BroadcastReceiver receiver = this;
      new Handler(Looper.getMainLooper()).post(() -> {
        Toast.makeText(context, message, Toast.LENGTH_LONG).show();
        context.unregisterReceiver(receiver);
      });
    }
  }
}
