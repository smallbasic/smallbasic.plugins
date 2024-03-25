package ioio.smallbasic;

import java.util.concurrent.atomic.AtomicBoolean;

import ioio.lib.api.DigitalInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class DigitalInputImpl extends IOTask implements DigitalInput {
  private static final String TAG = "DigitalInput";
  private final AtomicBoolean value = new AtomicBoolean();
  private DigitalInput input;

  public DigitalInputImpl() {
    super();
    Log.i(TAG, "created DigitalInput");
  }

  @Override
  public void close() {
    super.close();
    if (input != null) {
      input.close();
      input = null;
    }
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    value.set(input.read());
  }

  @Override
  public boolean read() {
    handleError();
    return value.get();
  }

  @Override
  public void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    input = ioio.openDigitalInput(pin);
  }

  @Override
  public void waitForValue(boolean value) {
    throw new UnsupportedOperationException();
  }
}
