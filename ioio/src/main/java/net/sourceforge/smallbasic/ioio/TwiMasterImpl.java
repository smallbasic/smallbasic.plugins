package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.IOIO;
import ioio.lib.api.TwiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class TwiMasterImpl extends IOTask {
  private static final String TAG = "TwiMasterImpl";
  private TwiMaster twiMaster = null;
  private TwiMaster.Rate rate = TwiMaster.Rate.RATE_100KHz;
  private int twiNum;
  private boolean smbus;

  public TwiMasterImpl() {
    super();
    Log.i(TAG, "created");
  }

  public void open(int twiNum, int smbus) {
    this.twiNum = twiNum;
    this.smbus = (smbus == 1);
  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {
    // TODO
  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    twiMaster = ioio.openTwiMaster(twiNum, rate, smbus);
  }
}
