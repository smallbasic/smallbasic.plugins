package net.sourceforge.smallbasic.ioio;

import net.sourceforge.smallbasic.ioio.input.AnalogInput;

class AnalogInputTest {
  public static void main(String[] args) throws InterruptedException {
    //jtermios.JTermios.JTermiosLogging.setLogLevel(4);
    AnalogInput input = new AnalogInput();
    input.openInput(1);
    Thread.sleep(30000);
  }
}