package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class DigitalInputImpl implements DigitalInput, IOTask {
  private static final String TAG = "DigitalInput";
  private CountDownLatch latch;
  private DigitalInput input;
  private int pin;
  private volatile boolean value;

  public DigitalInputImpl() {
    super();
    latch = null;
    Log.i(TAG, "created DigitalInput");
  }

  @Override
  public void close() {
    IOService.getInstance().removeTask(this);
    input.close();
    input = null;
  }

  @Override
  public int getPin() {
    return pin;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    value = input.read();
  }

  public void open(int pin) throws IOException {
    Log.i(TAG, "open");
    this.pin = pin;
    IOService.getInstance().addTask(this);
  }

  @Override
  public boolean read() {
    return value;
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      input = ioio.openDigitalInput(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  public void waitForValue(boolean value) throws InterruptedException, ConnectionLostException {
  }
}
