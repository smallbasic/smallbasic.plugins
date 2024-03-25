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

import ioio.smallbasic.IOIOException;

import java.util.concurrent.CountDownLatch;

import ioio.lib.spi.Log;

public class AccessoryPermissionManager extends BroadcastReceiver {
  private static final String TAG = AccessoryPermissionManager.class.getSimpleName();
  private static final String ACTION_USB_PERMISSION = "ioio.lib.accessory.action.USB_PERMISSION";
  private static final String EXTRA_PERMISSION_GRANTED = UsbManager.EXTRA_PERMISSION_GRANTED;

  private final UsbManager usbManager;
  private final Context context;
  private final CountDownLatch latch;
  private boolean hasPermission;

  public AccessoryPermissionManager() throws InterruptedException {
    Log.d(TAG, "validating accessory permission");
    this.context = IOIOLoader.getContext();
    this.usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
    this.latch = new CountDownLatch(1);
    this.hasPermission = false;
    debug("in validate");
    validate();
    debug("done validate");
 }

  @Override
  public void onReceive(Context context, Intent intent) {
    final String action = intent.getAction();
    if (ACTION_USB_PERMISSION.equals(action)) {
      hasPermission = intent.getBooleanExtra(EXTRA_PERMISSION_GRANTED, false);
      debug("has perm " + hasPermission) ;
      Log.v(TAG, "Permission: " + (hasPermission ? "granted" : "denied"));
      context.unregisterReceiver(this);
//      latch.countDown();
    }
  }

  @TargetApi(Build.VERSION_CODES.TIRAMISU)
  private void validate() throws InterruptedException {
    UsbAccessory[] accessories = usbManager.getAccessoryList();
    UsbAccessory accessory = (accessories == null ? null : accessories[0]);
    if (accessory == null) {
      throw new IOIOException("No accessory found.");
    }
    if (usbManager.hasPermission(accessory)) {
      debug("already granted");
      Log.v(TAG, "Permission already granted.");
    } else {
      Log.v(TAG, "Requesting permission.");
      IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
      context.registerReceiver(this, filter, Context.RECEIVER_NOT_EXPORTED);
      int flags = PendingIntent.FLAG_IMMUTABLE;
      Intent intent = new Intent(ACTION_USB_PERMISSION);
      PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
      usbManager.requestPermission(accessory, pendingIntent);
      debug("about to wait");
      latch.await();
      debug("done waiting");
      if (!hasPermission) {
        throw new IOIOException("Permission denied.");
      }
    }
  }

  private void debug(String message) {
//    ((Activity)context).runOnUiThread(new Runnable() {
//      @Override
//      public void run() {
//        Toast.makeText(context, message.trim(), Toast.LENGTH_LONG).show();
//      }
//    });
  }
}
