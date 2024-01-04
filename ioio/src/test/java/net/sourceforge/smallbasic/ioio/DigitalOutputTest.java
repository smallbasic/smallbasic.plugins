package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;

class DigitalOutputTest {
  public static void main(String[] args) throws InterruptedException {
    DigitalOutput output = new DigitalOutput();
    output.open(IOIO.LED_PIN);
    System.err.println("done open");
    int value = 1;
    while (true) {
      System.err.println("Setting LED " + value);
      output.write(value);
      value = value == 1 ? 0 : 1;
      Thread.sleep(1000);
    }
  }
}
