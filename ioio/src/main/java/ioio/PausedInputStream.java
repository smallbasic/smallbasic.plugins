package ioio;

import java.io.IOException;
import java.io.InputStream;

/**
 * Pause between read() to avoid excessive CPU usage
 */
public class PausedInputStream extends InputStream {
  private long lastAccessMillis;
  private final InputStream wrapped;
  private boolean closed;

  public PausedInputStream(InputStream inputStream) {
    this.wrapped = inputStream;
    this.lastAccessMillis = System.currentTimeMillis();
    this.closed = false;
  }

  @Override
  public void close() throws IOException {
    wrapped.close();
    closed = true;
  }

  @Override
  public synchronized int read(byte[] bytes, int offset, int len) throws IOException {
    throw new UnsupportedOperationException();
  }

  @Override
  public int read(byte[] bytes) throws IOException {
    pause();
    int result = -1;
    while (!closed) {
      result = wrapped.read(bytes);
      if (result > 0) {
        break;
      }
    }
    return result;
  }

  @Override
  public synchronized int read() throws IOException {
    throw new UnsupportedOperationException();
  }

  private void pause() throws IOException {
    try {
      lastAccessMillis = TimerUtil.tick(lastAccessMillis);
    } catch (InterruptedException e) {
      throw new IOException(e);
    }
  }
}
