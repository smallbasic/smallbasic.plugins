package ioio.smallbasic.pc;

public class HexDump {
  private int index;
  private final String prefix;

  HexDump(String prefix) {
    this.index = 0;
    this.prefix = prefix;
  }

  void print(int data) {
    if (index % 16 == 0) {
      if (index != 0) {
        System.out.println();
      }
      System.out.printf("   %08x  ", index);
    }
    System.out.printf("%s:%02x ", prefix, data);
    index++;
  }
}
