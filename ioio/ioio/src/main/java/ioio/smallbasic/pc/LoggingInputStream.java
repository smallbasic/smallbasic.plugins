package ioio.smallbasic.pc;

import java.io.IOException;
import java.io.InputStream;

public class LoggingInputStream extends InputStream {
  private static final int DEBUG_OUT = 0x24;
  private final InputStream wrappedStream;
  private final HexDump hexDump;

  public LoggingInputStream(InputStream wrappedStream) {
    this.wrappedStream = wrappedStream;
    this.hexDump = new HexDump("RX");
  }

  @Override
  public int read() throws IOException {
    // see: IOIOLibCore/src/main/java/ioio/lib/impl/IOIOProtocol.java
    int data = wrappedStream.read();
    if (data == DEBUG_OUT) {
      System.out.print("[FIRMWARE] ");
      data = wrappedStream.read();
      while (data != '\n') {
        System.out.print(Character.valueOf((char) data));
        data = wrappedStream.read();
      }
      System.out.println();
      data = read();
    } else {
      hexDump.print(data);
    }
    return data;
  }

  @Override
  public void close() throws IOException {
    wrappedStream.close();
  }
}

