package net.sourceforge.smallbasic.ioio;

import ioio.TimerUtil;
import ioio.lib.api.IOIO;
import ioio.lib.spi.Log;

import java.io.IOException;

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
    TimerUtil.setLatency(latency);
    IOService.getInstance().start();
    lock.invoke(IOIO::waitForConnect);
  }

  public void waitForDisconnect() {
    lock.invoke(IOIO::waitForDisconnect);
  }
}
