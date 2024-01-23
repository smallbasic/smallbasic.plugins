package net.sourceforge.smallbasic.ioio;

import java.util.concurrent.atomic.AtomicBoolean;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class DigitalOutputImpl extends IOTask implements DigitalOutput {
  private static final String TAG = "DigitalOutput";
  private final AtomicBoolean value = new AtomicBoolean(false);
  private DigitalOutput output;

  public DigitalOutputImpl() {
    super();
    Log.i(TAG, "created DigitalOutput");
  }

  @Override
  public void close() {
    super.close();
    output.close();
    output = null;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    output.write(value.get());
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      output = ioio.openDigitalOutput(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  public void write(boolean value) throws ConnectionLostException {
    this.value.set(value);
  }
}