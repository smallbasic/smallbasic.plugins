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

  public int readWrite(int address, byte[] write) {
    handleError();
    return lock.invokeInt((i) -> {
      byte[] read = new byte[4];
      twiMaster.writeRead(address, false, write, write.length, read, read.length);
      Log.i(TAG, "read = " + read.length + " " + address + " "
        + read[0] + " " + read[1] + " " + read[2] + " " + read[3]);
      int value = 0; // TODO read read into value
      return value;
    });
  }

  public void write(int address, byte[] write) {
    handleError();
    lock.invoke((i) -> {
      twiMaster.writeRead(address, false, write, write.length, null, 0);
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
