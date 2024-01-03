package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;

class DigitalOutputTest {
  public static void main(String[] args) throws InterruptedException {
    DigitalOutput output = new DigitalOutput();
    output.open(IOIO.LED_PIN);
    boolean value = true;
    while (true) {
      output.setValue(value);
      value = !value;
      Thread.sleep(1000);
    }
  }
}
