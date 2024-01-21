package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

import ioio.lib.api.DigitalOutput;
import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class DigitalOutputImpl implements DigitalOutput, IOTask {
  private static final String TAG = "DigitalOutput";
  private DigitalOutput output;
  private volatile boolean value;
  private int pin;

  public DigitalOutputImpl() {
    super();
    Log.i(TAG, "created DigitalOutput");
  }

  @Override
  public void close() {
    IOService.getInstance().removeTask(this);
    output.close();
    output = null;
  }

  @Override
  public int getPin() {
    return pin;
  }

  @Override
  public void loop() throws InterruptedException, ConnectionLostException {
    output.write(value);
  }

  public void open(int pin) throws IOException {
    Log.i(TAG, "open");
    this.pin = pin;
    IOService.getInstance().addTask(this);
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
    this.value = value;
  }
}
