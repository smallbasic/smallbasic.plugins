package ioio.smallbasic;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.spi.Log;

public class IOIOImpl extends IOTask {
  private static final String TAG = "IOIOImpl";
  private final IOLock<IOIO> lock = new IOLock<>();
  private IOIO ioio;

  public IOIOImpl() throws IOException {
    Log.i(TAG, "created");
    IOService.getInstance().addTask(this);
  }

  public void beginBatch() {
    lock.invoke(IOIO::beginBatch);
  }

  public void close() {
    super.close();
    IOService.getInstance().stop();
  }

  public void disconnect() {
    lock.invoke(IOIO::disconnect);
  }

  public void endBatch() {
    lock.invoke(IOIO::endBatch);
  }

  @Override
  public int getPin() {
    return -1;
  }

  public void hardReset() {
    lock.invoke(IOIO::hardReset);
  }

  @Override
  public void loop() {
    lock.process(ioio);
  }

  @Override
  public void setup(IOIO ioio) {
    this.ioio = ioio;
  }

  public void softReset() {
    lock.invoke(IOIO::softReset);
  }

  public void sync() {
    lock.invoke(IOIO::sync);
  }

  public void waitForConnect(int latency) {
    IOUtil.setError(null);
    if (latency < 0) {
      IOUtil.setHardReset(true);
    } else {
      IOUtil.setHardReset(false);
      TimerUtil.setLatency(latency);
    }
    IOService.getInstance().start();
    handleError();
    lock.invoke(IOIO::waitForConnect);
    handleError();
  }

  public void waitForDisconnect() {
    lock.invoke(IOIO::waitForDisconnect);
  }
}
