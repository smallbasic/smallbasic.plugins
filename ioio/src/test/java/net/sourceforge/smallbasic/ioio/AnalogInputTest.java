package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

class AnalogInputTest {
  public static void main(String[] args) throws InterruptedException, IOException {
    //jtermios.JTermios.JTermiosLogging.setLogLevel(4);
    AnalogInputImpl input = new AnalogInputImpl();
    input.open(1);
    Thread.sleep(30000);
  }
}