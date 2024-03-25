package ioio.smallbasic;

import ioio.lib.api.CapSense;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class CapSenseImpl extends IOTask implements CapSense {
  private static final String TAG = "CapSense";
  private final IOLock<CapSense> lock = new IOLock<>();
  private CapSense capSense;

  public CapSenseImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    if (capSense != null) {
      capSense.close();
      capSense = null;
    }
  }

  @Override
  public void loop() {
    lock.process(capSense);
  }

  @Override
  public float read() {
    return lock.invoke(CapSense::read);
  }

  @Override
  public float readSync() {
    return lock.invoke(CapSense::readSync);
  }

  @Override
  public void setFilterCoef(float t) {
    throw new UnsupportedOperationException();
  }

  @Override
  public void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    capSense = ioio.openCapSense(pin);
  }

  @Override
  public void waitOver(float threshold) {
    lock.invoke((i) -> {
      capSense.waitOver(threshold);
    });
  }

  @Override
  public void waitOverSync(float threshold) {
    lock.invoke((i) -> {
      capSense.waitOverSync(threshold);
    });
  }

  @Override
  public void waitUnder(float threshold) {
    lock.invoke((i) -> {
      capSense.waitUnder(threshold);
    });
  }

  @Override
  public void waitUnderSync(float threshold) {
    lock.invoke((i) -> {
      capSense.waitUnderSync(threshold);
    });
  }
}

