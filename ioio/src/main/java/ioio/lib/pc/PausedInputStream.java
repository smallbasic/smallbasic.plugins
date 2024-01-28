package ioio.lib.pc;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Pause between read() to avoid excessive CPU usage
 */
class PausedInputStream extends BufferedInputStream {
  private long lastAccessMillis;

  public PausedInputStream(InputStream inputStream) {
    super(inputStream);
    lastAccessMillis = System.currentTimeMillis();
  }

  @Override
  public synchronized int read(byte[] bytes, int i, int i1) throws IOException {
    pause();
    return super.read(bytes, i, i1);
  }

  @Override
  public int read(byte[] bytes) throws IOException {
    pause();
    return super.read(bytes);
  }

  @Override
  public synchronized int read() throws IOException {
    pause();
    return super.read();
  }

  private void pause() throws IOException {
    try {
      lastAccessMillis = Timer.tick(lastAccessMillis);
    } catch (InterruptedException e) {
      throw new IOException(e);
    }
  }
}
