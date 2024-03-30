package ioio.smallbasic;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.exception.ConnectionLostException;

class DigitalOutputTest {
  public static void main(String[] args) throws InterruptedException, ConnectionLostException, IOException {
    DigitalOutputImpl output = new DigitalOutputImpl();
    output.open(IOIO.LED_PIN);
    IOService.getInstance().start();
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
