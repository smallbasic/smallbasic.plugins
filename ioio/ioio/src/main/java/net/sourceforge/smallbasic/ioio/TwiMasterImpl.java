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
    if (twiMaster != null) {
      twiMaster.close();
      twiMaster = null;
    }
  }

  public void open(int twiNum, int smbus) throws IOException {
    super.open(twiNum);
    this.twiNum = twiNum;
    this.smbus = (smbus == 1);
  }

  public long readWrite(int address, int readLen, final byte[] write, int writeLen) {
    handleError();
    return lock.invokeLong((i) -> {
      byte[] read = new byte[readLen];
      twiMaster.writeRead(address, false, write, writeLen, read, read.length);
      long result = 0;
      for (int index = 0; index < read.length; index++) {
        result += ((long)Byte.toUnsignedInt(read[index])) << (index * 8);
      }
      return result;
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
