package ioio.smallbasic.android;

import android.content.Context;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.ParcelFileDescriptor;

public class UsbUtil {
  private UsbUtil() {
    // no access
  }

  static ParcelFileDescriptor getParcelFileDescriptor() {
    return getUsbManager().openAccessory(getUsbAccessory());
  }

  static UsbAccessory getUsbAccessory() {
    UsbManager usbManager = (UsbManager) IOIOLoader.getContext().getSystemService(Context.USB_SERVICE);
    UsbAccessory[] accessories = usbManager.getAccessoryList();
    return (accessories == null ? null : accessories[0]);
  }

  static UsbManager getUsbManager() {
    Context activity = IOIOLoader.getContext();
    return (UsbManager) activity.getSystemService(Context.USB_SERVICE);
  }
}
