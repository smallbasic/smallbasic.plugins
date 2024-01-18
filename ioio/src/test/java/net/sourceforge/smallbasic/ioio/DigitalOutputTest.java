package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

class DigitalOutputTest {
  public static void main(String[] args) throws InterruptedException, ConnectionLostException {
    DigitalOutputImpl output = new DigitalOutputImpl();
    output.open(IOIO.LED_PIN);
    System.err.println("done open");
    boolean value = true;
    while (true) {
      System.err.println("Setting LED " + value);
      output.write(value);
      value = !value;
      Thread.sleep(1000);
    }
  }
}
