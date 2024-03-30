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
    if (accessory == null) {
      throw new IOIOException("No usb accessory found.");
    }

    UsbManager usbManager = (UsbManager) IOIOLoader.getContext().getSystemService(Context.USB_SERVICE);
    if (!usbManager.hasPermission(accessory)) {
      new Handler(Looper.getMainLooper()).post(() -> {
        Context context = IOIOLoader.getContext();
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        filter.setPriority(IntentFilter.SYSTEM_HIGH_PRIORITY - 1);
        context.registerReceiver(this, filter, Context.RECEIVER_NOT_EXPORTED);
        int flags = PendingIntent.FLAG_IMMUTABLE;
        Intent intent = new Intent(ACTION_USB_PERMISSION);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
        usbManager.requestPermission(accessory, pendingIntent);
      });
      // for some reason using a latch here caused an ANR
      Log.d(TAG, "requesting permission");
      throw new IOIOException(PERMISSION_ERROR);
    }
  }

  @Override
  public synchronized void onReceive(final Context context, Intent intent) {
    Log.d(TAG, "onReceive entered");
    if (ACTION_USB_PERMISSION.equals(intent.getAction())) {
      final BroadcastReceiver receiver = this;
      new Handler(Looper.getMainLooper()).post(() -> {
        context.unregisterReceiver(receiver);
      });
    }
  }
}
