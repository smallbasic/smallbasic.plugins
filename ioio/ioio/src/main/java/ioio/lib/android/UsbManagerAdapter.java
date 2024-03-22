/*
 * Copyright 2014 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

package ioio.lib.android;

import android.app.PendingIntent;
import android.content.Context;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.ParcelFileDescriptor;

import ioio.lib.spi.Log;
import ioio.lib.spi.NoRuntimeSupportException;

/**
 * This class serves as a common interface for USB accessory for android.hardware.usb.UsbManager
 *
 * When this class is instantiated, it will throw a {@link NoRuntimeSupportException} if the
 * underlying implementation does not exist.
 */
class UsbManagerAdapter {
  private static final String TAG = UsbManagerAdapter.class.getSimpleName();

  UsbManagerAdapter() throws NoRuntimeSupportException {
    try {
      Class.forName("android.hardware.usb.UsbManager");
    } catch (ClassNotFoundException e) {
      Log.d(TAG, e.toString());
      throw new NoRuntimeSupportException("No support for USB accessory mode.");
    }
  }

  AbstractUsbManager getManager(Context wrapper) {
    final UsbManager manager = (UsbManager) wrapper.getSystemService(Context.USB_SERVICE);
    return new UsbManagerImpl(manager);
  }

  static abstract class AbstractUsbManager {
    final String EXTRA_PERMISSION_GRANTED;
    protected AbstractUsbManager() {
      EXTRA_PERMISSION_GRANTED = UsbManager.EXTRA_PERMISSION_GRANTED;
    }

    abstract UsbAccessoryInterface[] getAccessoryList();
    abstract boolean hasPermission(UsbAccessoryInterface accessory);
    abstract ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory);
    abstract void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent);
  }

  private static final class UsbManagerImpl extends AbstractUsbManager {
    private final UsbManager manager;

    private UsbManagerImpl(UsbManager manager) {
      super();
      this.manager = manager;
    }

    @Override
    UsbAccessoryInterface[] getAccessoryList() {
      final UsbAccessory[] accessoryList = manager.getAccessoryList();
      if (accessoryList == null) {
        return null;
      }
      UsbAccessoryInterface[] result = new UsbAccessoryInterface[accessoryList.length];
      for (int i = 0; i < accessoryList.length; ++i) {
        result[i] = new UsbAccessoryAdapter<>(accessoryList[i]);
      }
      return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    boolean hasPermission(UsbAccessoryInterface accessory) {
      return manager.hasPermission(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
    }

    @SuppressWarnings("unchecked")
    @Override
    ParcelFileDescriptor openAccessory(UsbAccessoryInterface accessory) {
      return manager.openAccessory(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl);
    }

    @SuppressWarnings("unchecked")
    @Override
    void requestPermission(UsbAccessoryInterface accessory, PendingIntent pendingIntent) {
      manager.requestPermission(((UsbAccessoryAdapter<UsbAccessory>) accessory).impl, pendingIntent);
    }
  }

  private static class UsbAccessoryAdapter<T> implements UsbAccessoryInterface {
    final T impl;
    UsbAccessoryAdapter(T t) {
      impl = t;
    }
  }

  interface UsbAccessoryInterface {
  }
}
