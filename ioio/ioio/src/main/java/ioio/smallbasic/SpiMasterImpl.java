package ioio.smallbasic;

import java.io.IOException;

import ioio.lib.api.IOIO;
import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class SpiMasterImpl extends IOTask {
  private static final String TAG = "SpiMasterImpl";
  private final IOLock<SpiMaster> lock = new IOLock<>();
  private SpiMaster spiMaster = null;
  private int miso;
  private int mosi;
  private int clk;
  private int slaveSelect;

  public SpiMasterImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public void close() {
    super.close();
    if (spiMaster != null) {
      spiMaster.close();
      spiMaster = null;
    }
  }

  public void open(int miso, int mosi, int clk, int slaveSelect) throws IOException {
    super.open(miso);
    this.miso = miso;
    this.mosi = mosi;
    this.clk = clk;
    this.slaveSelect = slaveSelect;
    validatePins();
  }

  public void write(int address, int data) {
    handleError();
    lock.invoke((i) -> {
      byte[] buffer = {(byte) address, (byte) data};
      spiMaster.writeRead(buffer, buffer.length, buffer.length, null, 0);
    });
  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {
    lock.process(spiMaster);
  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered: " + miso + " " + mosi + " " + clk + " " + slaveSelect);
    spiMaster = ioio.openSpiMaster(miso, mosi, clk, slaveSelect, SpiMaster.Rate.RATE_1M);
  }

  private void pinError(String name) {
    IOUtil.setError("Incorrect " + name + " pin value");
  }

  private void validatePins() {
    if (miso < 1) {
      pinError("miso");
    } else if (mosi < 1) {
      pinError("mosi");
    } else if (clk < 1) {
      pinError("clk");
    } else if (slaveSelect < 1) {
      pinError("slaveSelect");
    } else if (miso == mosi ||
               miso == clk ||
               miso == slaveSelect ||
               mosi == clk ||
               mosi == slaveSelect ||
               clk == slaveSelect) {
      IOUtil.setError("One or pins have duplicate values");
    }
  }
}
