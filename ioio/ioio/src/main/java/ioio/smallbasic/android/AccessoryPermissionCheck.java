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

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import ioio.lib.spi.Log;
import ioio.smallbasic.IOIOException;

public class AccessoryPermissionCheck extends BroadcastReceiver {
  private static final String TAG = AccessoryPermissionCheck.class.getSimpleName();
  private static final String ACTION_USB_PERMISSION = "android.hardware.usb.action.USB_PERMISSION";
  private static final String PERMISSION_ERROR = "Not permitted to use usb accessory.";
  private static final long TIMEOUT_SECS = 30;
  private final CountDownLatch latch;
  private final AtomicBoolean permitted;

  @TargetApi(Build.VERSION_CODES.TIRAMISU)
  public AccessoryPermissionCheck() {
    Log.d(TAG, "AccessoryPermissionCheck entered");
    this.permitted = new AtomicBoolean(false);
    this.latch = new CountDownLatch(1);

    UsbManager usbManager = (UsbManager) IOIOLoader.getContext().getSystemService(Context.USB_SERVICE);
    UsbAccessory[] accessories = usbManager.getAccessoryList();
    UsbAccessory accessory = (accessories == null ? null : accessories[0]);
    if (accessory == null) {
      throw new IOIOException("No usb accessory found.");
    }
    if (!usbManager.hasPermission(accessory)) {
      new Handler(Looper.getMainLooper()).post(() -> {
        Context context = IOIOLoader.getContext();
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        context.registerReceiver(this, filter, Context.RECEIVER_EXPORTED);
        int flags = PendingIntent.FLAG_IMMUTABLE;
        Intent intent = new Intent(ACTION_USB_PERMISSION);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
        usbManager.requestPermission(accessory, pendingIntent);
      });
      Log.d(TAG, "waiting for permission");
      try {
        if (!latch.await(TIMEOUT_SECS, TimeUnit.SECONDS)) {
          permissionError();
        } else {
          IOIOLoader.getContext().unregisterReceiver(this);
        }
      }
      catch (InterruptedException e) {
        permissionError();
        throw new IOIOException(PERMISSION_ERROR);
      }
      if (!permitted.get()) {
        permissionError();
      }
    }
  }

  @Override
  public void onReceive(Context context, Intent intent) {
    Log.d(TAG, intent.getAction());
    synchronized (this) {
      if (ACTION_USB_PERMISSION.equals(intent.getAction())) {
        permitted.set(intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false));
        latch.countDown();
      }
    }
  }

  private void permissionError() {
    IOIOLoader.getContext().unregisterReceiver(this);
    throw new IOIOException(PERMISSION_ERROR);
  }
}
