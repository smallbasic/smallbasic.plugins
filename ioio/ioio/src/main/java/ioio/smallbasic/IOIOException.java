package ioio.smallbasic;

public class IOIOException extends RuntimeException {
  public IOIOException() {
    super();
  }

  public IOIOException(Exception exception) {
    this(exception.getMessage());
  }

  public IOIOException(String message) {
    super(message);
    IOUtil.setError(message);
  }

  public String getMessage() {
    return IOUtil.getError();
  }
}
