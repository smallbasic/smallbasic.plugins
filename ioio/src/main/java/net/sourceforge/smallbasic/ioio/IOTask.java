package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

/**
 * IOTask - methods invoked in the looper thread
 * Other task methods are invoked from the SB thread do must use
 * thread synchronization for safe access
 */
public interface IOTask {
  int getPin();
  void loop() throws ConnectionLostException, InterruptedException;
  void setup(IOIO ioio);
}
