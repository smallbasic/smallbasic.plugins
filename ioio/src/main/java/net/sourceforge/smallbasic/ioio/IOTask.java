package net.sourceforge.smallbasic.ioio;

import java.io.Closeable;
import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

/**
 * IOTask - setup() and loop() are invoked in the looper thread
 * whereas specific task methods are invoked from the SB thread
 * thus require appropriate thread synchronization handling.
 */
public abstract class IOTask implements Closeable {
  protected int pin;

  @Override
  public void close() {
    IOService.getInstance().removeTask(this);
  }

  public void open(int pin) throws IOException {
    this.pin = pin;
    IOService.getInstance().addTask(this);
  }

  int getPin() {
    return pin;
  }

  /**
   * Subclasses should override this method for performing operations to be
   * done repetitively as long as IOIO communication persists. Typically, this
   * will be the main logic of the application, processing inputs and
   * producing outputs.
   */
  abstract void loop() throws ConnectionLostException, InterruptedException;

  /**
   * Subclasses should override this method for performing operations to be
   * done once as soon as IOIO communication is established.
   */
  abstract void setup(IOIO ioio);
}
