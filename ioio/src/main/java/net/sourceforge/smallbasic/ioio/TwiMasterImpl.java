package net.sourceforge.smallbasic.ioio;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.TwiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class TwiMasterImpl extends IOTask {
  private static final String TAG = "TwiMasterImpl";
  private final IOLock<TwiMaster> lock = new IOLock<>();
  private final TwiMaster.Rate rate = TwiMaster.Rate.RATE_100KHz;
  private TwiMaster twiMaster = null;
  private int twiNum;
  private boolean smbus;

  public TwiMasterImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    twiMaster.close();
    twiMaster = null;
  }

  public void open(int twiNum, int smbus) throws IOException {
    super.open(twiNum);
    this.twiNum = twiNum;
    this.smbus = (smbus == 1);
  }

  public int readWrite(int address, final byte[] write, int len) {
    handleError();
    return lock.invokeInt((i) -> {
      byte[] read = new byte[2];
      twiMaster.writeRead(address, false, write, len, read, read.length);
      return (read[1] << 8) + read[0];
    });
  }

  public void write(int address, final byte[] write, int len) {
    handleError();
    lock.invoke((i) -> {
      twiMaster.writeRead(address, false, write, len, null, 0);
    });
  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {
    lock.process(twiMaster);
  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered: " + twiNum);
    twiMaster = ioio.openTwiMaster(twiNum, rate, smbus);
  }
}