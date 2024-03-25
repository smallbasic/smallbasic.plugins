package ioio.smallbasic;

import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.Sequencer;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

public class SequencerImpl extends IOTask implements Sequencer {
  private static final String TAG = "Sequencer";
  private final IOLock<AnalogInput> lock = new IOLock<>();
  private Sequencer input = null;

  public SequencerImpl() {
    super();
    Log.i(TAG, "created");
  }

  @Override
  public int available() {
    return 0; // lock.invokeInt(Sequencer::available);
  }

  @Override
  public Event getLastEvent() throws ConnectionLostException {
    return null;
  }

  @Override
  public void manualStart(ChannelCue[] cues) throws ConnectionLostException {

  }

  @Override
  public void manualStop() throws ConnectionLostException {

  }

  @Override
  public void pause() throws ConnectionLostException {

  }

  @Override
  public void push(ChannelCue[] cues, int duration) throws
                                                    ConnectionLostException,
                                                    InterruptedException {

  }

  @Override
  public void setEventQueueSize(int size) throws ConnectionLostException {

  }

  @Override
  public void start() throws ConnectionLostException {

  }

  @Override
  public void stop() throws ConnectionLostException {

  }

  @Override
  public Event waitEvent() throws ConnectionLostException, InterruptedException {
    return null;
  }

  @Override
  public void waitEventType(Event.Type type) throws ConnectionLostException, InterruptedException {

  }

  @Override
  void loop() throws ConnectionLostException, InterruptedException {

  }

  @Override
  void setup(IOIO ioio) throws ConnectionLostException {
    Log.i(TAG, "setup entered");
    //input = ioio.openSequencer(pin);
  }
}
