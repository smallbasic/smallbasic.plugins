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

import android.os.ParcelFileDescriptor;

import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;
import ioio.smallbasic.IOIOException;
import ioio.smallbasic.IOService;

class BluetoothConnection implements IOIOConnection {
  private static final String TAG = BluetoothConnection.class.getSimpleName();
  private static final int HARD_RESET = 0x00;
  private static final int SOFT_RESET = 0x01;
  private static final int ESTABLISH_CONNECTION = 0x00;

  private ConnectionState state;
  private FixedReadBufferedInputStream inputStream;
  private OutputStream outputStream;
  private ParcelFileDescriptor fileDescriptor;

  private enum ConnectionState {
    INIT, CONNECTED, DISCONNECTED
  }

  BluetoothConnection(ParcelFileDescriptor fileDescriptor) {
    Log.i(TAG, "creating BluetoothConnection");
    this.fileDescriptor = fileDescriptor;
    this.state = ConnectionState.INIT;
    this.inputStream = null;
    this.outputStream = null;
    if (this.fileDescriptor == null) {
      throw new IOIOException("Failed to obtain descriptor");
    }
  }

  @Override
  public boolean canClose() {
    return false;
  }

  @Override
  public synchronized void disconnect() {
    Log.i(TAG, "disconnect entered");
    this.state = ConnectionState.DISCONNECTED;
    if (fileDescriptor != null) {
      try {
        fileDescriptor.close();
      } catch (IOException e) {
        Log.e(TAG, "Failed to close file descriptor.", e);
      }
      fileDescriptor = null;
    }
    Log.d(TAG, "leaving disconnect");
  }

  @Override
  public InputStream getInputStream() {
    return inputStream;
  }

  @Override
  public OutputStream getOutputStream() {
    return outputStream;
  }

  @Override
  public synchronized void waitForConnect() throws ConnectionLostException {
    if (state != ConnectionState.INIT) {
      throw new IllegalStateException("waitForConnect() may only be called once");
    }
    if (open()) {
      state = ConnectionState.CONNECTED;
    } else {
      state = ConnectionState.DISCONNECTED;
      throw new ConnectionLostException();
    }
  }

  @Override
  protected void finalize() {
    disconnect();
  }

  private boolean open() {
    boolean result = false;
    Log.i(TAG, "open() entered");

    try {
      FileDescriptor fd = fileDescriptor.getFileDescriptor();
      // If data is read from the InputStream created from this file descriptor all
      // data of a USB transfer should be read at once.
      inputStream = new FixedReadBufferedInputStream(new FileInputStream(fd), 1024);
      outputStream = new BufferedOutputStream(new FileOutputStream(fd), 1024);

      // open the connection
      if (IOService.getHardReset()) {
        outputStream.write(HARD_RESET);
        outputStream.write('I');
        outputStream.write('O');
        outputStream.write('I');
        outputStream.write('O');
        IOService.setHardReset(false);
      } else {
        outputStream.write(SOFT_RESET);
      }
      outputStream.flush();

      // ensure IOIOProtocol.run() first see's ESTABLISH_CONNECTION(0x00) and not SOFT_RESET(0x01)
      // to avoid an NPE in IncomingState.handleSoftReset(). This method relies on initialisation in
      // IncomingState.handleEstablishConnection
      if (inputStream.positionTo(ESTABLISH_CONNECTION)) {
        Log.i(TAG, "created streams");
      } else {
        Log.i(TAG, "created streams - failed to position data");
      }
      result = true;
    } catch (Exception e) {
      Log.v(TAG, "Failed to open streams", e);
    } finally {
      if (!result) {
        disconnect();
      }
    }
    return result;
  }
}
