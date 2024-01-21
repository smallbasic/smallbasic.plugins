package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

import java.util.concurrent.CountDownLatch;

public class DigitalInputImpl extends IOTask implements DigitalInput {
  private static final String TAG = "DigitalInput";
  private CountDownLatch latch;
  private DigitalInput input;
  private volatile boolean value;

  public DigitalInputImpl() {
    super();
    latch = null;
    Log.i(TAG, "created DigitalInput");
  }

  @Override
  public void close() {
    super.close();
    input.close();
    input = null;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    value = input.read();
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
