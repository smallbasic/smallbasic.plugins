package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

import ioio.lib.api.CapSense;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class CapSenseImpl implements CapSense, IOTask {
  private static final String TAG = "CapSense";
  private CapSense capSense;
  private int pin;

  public CapSenseImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    IOService.getInstance().removeTask(this);
    capSense.close();
    capSense = null;
  }

  @Override
  public int getPin() {
    return pin;
  }

  @Override
  public void loop() throws ConnectionLostException, InterruptedException {
  }

  public void open(int pin) throws IOException {
    Log.i(TAG, "open");
    this.pin = pin;
    IOService.getInstance().addTask(this);
  }

  @Override
  public float read() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public float readSync() throws InterruptedException, ConnectionLostException {
    return 0;
  }

  @Override
  public void setFilterCoef(float t) throws ConnectionLostException {

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

