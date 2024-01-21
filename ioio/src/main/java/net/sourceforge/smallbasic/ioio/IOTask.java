package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

public interface IOTask {
  void loop() throws ConnectionLostException, InterruptedException;
  void setup(IOIO ioio);
}
