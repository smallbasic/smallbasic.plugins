package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.CapSense;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class CapSenseImpl extends IOTask implements CapSense {
  private static final String TAG = "CapSense";
  private static final IOLock<CapSense> lock = new IOLock<>();
  private CapSense capSense;

  public CapSenseImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    capSense.close();
    capSense = null;
  }

  @Override
  public void loop() throws ConnectionLostException, InterruptedException {
    lock.process(capSense);
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return lock.invoke(CapSense::read);
  }

  @Override
  public float readSync() throws InterruptedException, ConnectionLostException {
    return lock.invoke(CapSense::readSync);
  }

  @Override
  public void setFilterCoef(float t) throws ConnectionLostException {
    throw new UnsupportedOperationException();
  }

  @Override
  public void setup(IOIO ioio) {
    Log.i(TAG, "setup entered");
    try {
      capSense = ioio.openCapSense(pin);
    }
    catch (ConnectionLostException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  public void waitOver(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitOverSync(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitUnder(float threshold) throws ConnectionLostException, InterruptedException {

  }

  @Override
  public void waitUnderSync(float threshold) throws ConnectionLostException, InterruptedException {

  }
}

