package ioio.smallbasic;

import ioio.lib.api.IOIO;
import ioio.lib.api.SpiMaster;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

import java.io.IOException;

public class SpiMasterImpl extends IOTask {
  private static final String TAG = "SpiMasterImpl";
  private static final int SPI_WRITE_MAX = 62;
  private final byte[] BATCH = new byte[SPI_WRITE_MAX];
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

  public long readWrite(int readLen, final byte[] write, int writeLen) {
    handleError();
    return lock.invokeLong((i) -> {
      byte[] read = new byte[readLen];
      spiMaster.writeRead(write, writeLen, writeLen, read, read.length);
      long result = 0;
      for (int index = 0; index < read.length; index++) {
        result += ((long)Byte.toUnsignedInt(read[index])) << (index * 8);
      }
      return result;
    });
  }

  public void write(final byte[] write, int writeLen) {
    handleError();
    lock.invoke((i) -> {
      if (writeLen > SPI_WRITE_MAX) {
        int srcPos = 0;
        while (srcPos < writeLen) {
          int batchLen = Math.min(writeLen - srcPos, 2);
          System.arraycopy(write, srcPos, BATCH, 0, batchLen);
          spiMaster.writeReadAsync(0, BATCH, batchLen, batchLen, null, 0);
          srcPos += batchLen;
        }
      } else {
        spiMaster.writeRead(write, writeLen, writeLen, null, 0);
      }
    });
  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {
    lock.process(spiMaster);
  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered: miso:" + miso + " mosi:" + mosi + " clk:" + clk + " cs:" + slaveSelect);
    spiMaster = ioio.openSpiMaster(miso, mosi, clk, slaveSelect, SpiMaster.Rate.RATE_4M);
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
      IOUtil.setError("One or more pins have duplicate values");
    }
  }
}
