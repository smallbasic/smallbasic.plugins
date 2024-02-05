package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class DigitalOutputImpl extends IOTask implements DigitalOutput {
  private static final String TAG = "DigitalOutput";
  private final IOLock<DigitalOutput> lock = new IOLock<>();
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
  public void loop() throws ConnectionLostException {
    lock.process(output);
  }

  @Override
  public void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    output = ioio.openDigitalOutput(pin);
  }

  @Override
  public void write(boolean value) {
    handleError();
    lock.invoke((i) -> {
      i.write(value);
    });
  }
}
