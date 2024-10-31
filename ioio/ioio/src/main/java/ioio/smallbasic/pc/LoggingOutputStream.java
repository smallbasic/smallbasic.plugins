package ioio.smallbasic.pc;

import java.io.IOException;
import java.io.OutputStream;

public class LoggingOutputStream extends OutputStream {
  private final OutputStream wrappedStream;
  private final HexDump hexDump;

  public LoggingOutputStream(OutputStream outputStream) {
    this.wrappedStream = outputStream;
    this.hexDump = new HexDump("TX");
  }

  @Override
  public void write(int b) throws IOException {
    wrappedStream.write(b);
    hexDump.print(b);
  }

  @Override
  public void flush() throws IOException {
    wrappedStream.flush();
  }

  @Override
  public void close() throws IOException {
    wrappedStream.close();
  }
}
