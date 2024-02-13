package net.sourceforge.smallbasic.ioio;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicInteger;

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

  public int read(int address) {
    handleError();
    AtomicInteger atomicLong = new AtomicInteger();
    lock.invoke((i) -> {
      byte[] buffer = new byte[4];
      twiMaster.writeRead(address, false, null, 0, buffer, 4);
      int value = 0; // TODO read buffer into value
      atomicLong.set(value);
    });
    return atomicLong.get();
  }

  public void write(int address, int data) {
    handleError();
    lock.invoke((i) -> {
      byte[] buffer = {(byte) data};
      twiMaster.writeRead(address, false, buffer, buffer.length, null, 0);
    });
  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {
    lock.process(twiMaster);
  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    twiMaster = ioio.openTwiMaster(twiNum, rate, smbus);
  }
}
