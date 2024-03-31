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
import ioio.lib.impl.FixedReadBufferedInputStream;
import ioio.lib.spi.Log;
import ioio.smallbasic.IOIOException;
import ioio.smallbasic.IOUtil;

class UsbConnection implements IOIOConnection {
  private static final String TAG = UsbConnection.class.getSimpleName();
  private static final int SOFT_RESET = 0x01;
  private static final int HARD_RESET = 0x00;
  private static final int SYNC = 0x23;
  private static final int MAX_RETRIES = 10;
  private ConnectionState state;
  private FixedReadBufferedInputStream inputStream;
  private OutputStream outputStream;
  private ParcelFileDescriptor fileDescriptor;

  private enum ConnectionState {
    INIT, CONNECTED, DISCONNECTED
  }

  UsbConnection() {
    Log.d(TAG, "creating UsbConnection");
    this.state = ConnectionState.INIT;
    this.inputStream = null;
    this.outputStream = null;
    this.fileDescriptor = null;
  }

  @Override
  public boolean canClose() {
    return false;
  }

  @Override
  public synchronized void disconnect() {
    Log.d(TAG, "disconnect entered: " + state);
    if (state != ConnectionState.DISCONNECTED) {
      IOUtil.setError("USB disconnected");
      close();
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
  public synchronized void waitForConnect() {
    if (state != ConnectionState.INIT) {
      throw new IllegalStateException("waitForConnect() may only be called once");
    }
    if (open()) {
      state = ConnectionState.CONNECTED;
    } else {
      throw new IOIOException("USB connection lost");
    }
  }

  @Override
  protected void finalize() {
    disconnect();
  }

  private void close() {
    Log.d(TAG, "close streams");
    state = ConnectionState.DISCONNECTED;
    try {
      if (inputStream != null) {
        inputStream.close();
      }
      if (outputStream != null) {
        outputStream.close();
      }
      if (fileDescriptor != null) {
        fileDescriptor.close();
      }
      inputStream = null;
      outputStream = null;
      fileDescriptor = null;
    } catch (java.io.IOException e) {
      IOUtil.setError("Failed to close file descriptor: " + e);
      Log.e(TAG, "Failed to close file descriptor.", e);
    }
  }

  private void handleResetResponse(int attempt) throws IOException {
    if (attempt < MAX_RETRIES) {
      int response = inputStream.read();
      Log.d(TAG, "Response:" + response + " available:" + inputStream.available() + " attempt:" + attempt);
      if (response != SOFT_RESET) {
        // unexpected
        if (inputStream.available() < 1) {
          try {
            Thread.sleep(100);
          }
          catch (InterruptedException e) {
            throw new IOIOException(e);
          }
        }
        handleResetResponse(attempt + 1);
      }
    } else {
      throw new IOIOException("USB connection failure");
    }
  }

  private boolean open() {
    boolean result = false;
    Log.d(TAG, "open() entered");

    try {
      openStreams();
      resetBoard();
      handleResetResponse(0);
      result = true;
    } catch (java.io.IOException e) {
      IOUtil.setError("Failed to open streams: " + e);
    } finally {
      if (!result) {
        close();
      }
    }
    return result;
  }

  private void openStreams() {
    this.fileDescriptor = UsbUtil.getParcelFileDescriptor();
    if (this.fileDescriptor == null) {
      throw new IOIOException("Failed to obtain descriptor");
    }
    FileDescriptor fd = fileDescriptor.getFileDescriptor();
    inputStream = new FixedReadBufferedInputStream(new FileInputStream(fd), 1024);
    outputStream = new BufferedOutputStream(new FileOutputStream(fd), 1024);
  }

  private void resetBoard() throws IOException {
    if (IOUtil.getHardReset()) {
      Log.d(TAG, "hard reset");
      outputStream.write(HARD_RESET);
      outputStream.write('I');
      outputStream.write('O');
      outputStream.write('I');
      outputStream.write('O');
    } else {
      Log.d(TAG, "soft reset");
      outputStream.write(SYNC);
    }
    outputStream.flush();
  }
}
