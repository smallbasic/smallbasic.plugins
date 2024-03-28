package ioio.smallbasic.pc;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.IOIOConnection;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class SerialPortIOIOConnection implements IOIOConnection {
  private static final String TAG = "SerialPortIOIOConnection";
  private static final int READ_TIMEOUT_MILLIS = 20000;
  private static final int BAUD_RATE = 115200;
  private final SerialPort serialPort;
  private InputStream inputStream;
  private OutputStream outputStream;
  private boolean abort;

  public SerialPortIOIOConnection(String portName) {
    serialPort = SerialPort.getCommPort(portName);
    serialPort.setBaudRate(BAUD_RATE);
    serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING | SerialPort.TIMEOUT_WRITE_BLOCKING, READ_TIMEOUT_MILLIS, 0);
    abort = false;
  }

  @Override
  public void waitForConnect() throws ConnectionLostException {
    if (!abort && serialPort.openPort()) {
      inputStream = serialPort.getInputStream();
      outputStream = serialPort.getOutputStream();
      serialPort.setDTR();
    } else {
      throw new ConnectionLostException();
    }
  }

  @Override
  synchronized public void disconnect() {
    abort = true;
    if (serialPort != null) {
      try {
        inputStream.close();
      } catch (IOException e) {
        Log.i(TAG, e.toString());
      }
      serialPort.closePort();
    }
  }

  @Override
  public InputStream getInputStream() throws ConnectionLostException {
    if (inputStream == null) {
      throw new ConnectionLostException();
    }
    return inputStream;
  }

  @Override
  public OutputStream getOutputStream() throws ConnectionLostException {
    if (outputStream == null) {
      throw new ConnectionLostException();
    }
    return outputStream;
  }

  @Override
  public boolean canClose() {
    return true;
  }
}
