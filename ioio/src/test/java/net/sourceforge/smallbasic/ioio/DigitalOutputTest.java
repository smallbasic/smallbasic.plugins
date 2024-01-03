package net.sourceforge.smallbasic.ioio;

import net.sourceforge.smallbasic.ioio.output.DigitalOutput;

import ioio.lib.api.IOIO;

class DigitalOutputTest {
  public static void main(String[] args) throws InterruptedException {
    DigitalOutput output = new DigitalOutput();
    output.openOutput(IOIO.LED_PIN);
    boolean value = true;
    while (true) {
      output.setValue(value);
      value = !value;
      Thread.sleep(1000);
    }
  }
}
