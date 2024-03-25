package ioio.smallbasic;

public class IOIOException extends RuntimeException {
  public IOIOException(Exception exception) {
    super(exception.getMessage());
  }

  public IOIOException(String message) {
    super(message);
  }

  public String getMessage() {
    String result = super.getMessage();
    if (result != null) {
      result = result.replace("ioio.smallbasic.IOIOException:", "").trim();
    }
    return result;
  }
}
