/*
 * Copyright 2015 Ytai Ben-Tsvi. All rights reserved.
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

package ioio.smallbasic.android;

import android.content.Context;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;

import java.util.Collection;

import ioio.lib.api.IOIOConnection;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.Log;
import ioio.lib.spi.NoRuntimeSupportException;

public class AccessoryConnectionBootstrap implements IOIOConnectionBootstrap, IOIOConnectionFactory {
  private static final String TAG = AccessoryConnectionBootstrap.class.getSimpleName();

  public AccessoryConnectionBootstrap() throws NoRuntimeSupportException {
    Log.d(TAG, "creating AccessoryConnectionBootstrap");
  }

  @Override
  public IOIOConnection createConnection() {
    Log.i(TAG, "createConnection");
    Context activity = IOIOLoader.getContext();
    UsbManager usbManager = (UsbManager) activity.getSystemService(Context.USB_SERVICE);
    UsbAccessory accessory = usbManager.getAccessoryList()[0];
    return new BluetoothConnection(getUsbManager().openAccessory(accessory));
  }

  @Override
  public Object getExtra() {
    return null;
  }

  @Override
  public void getFactories(Collection<IOIOConnectionFactory> result) {
    result.add(this);
  }

  @Override
  public String getType() {
    return BluetoothConnection.class.getCanonicalName();
  }

  private UsbManager getUsbManager() {
    Context activity = IOIOLoader.getContext();
    return (UsbManager) activity.getSystemService(Context.USB_SERVICE);
  }
}
