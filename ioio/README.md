# IOIO for SmallBASIC

see: https://github.com/ytai/ioio/wiki

## IOIO

This interface provides control over all the IOIO board functions.

| Name    | Description   |
|---------|---------------|
|void beginBatch(void)|Start a batch of operations. This is strictly an optimization and will not change functionality.|
|void disconnect(void)|Closes the connection to the board, or aborts a connection process started with waitForConnect().|
|void endBatch(void)|End a batch of operations.|
|void hardReset(void)|Equivalent to disconnecting and reconnecting the board power supply.|
|void softReset(void)|Resets the entire state (returning to initial state), without dropping the connection.|
|void sync(void)|Sends a message to the IOIO and waits for an echo.|
|void waitForConnect(void)|Establishes connection with the IOIO board.|
|void waitForDisconnect(void)|Blocks until IOIO has been disconnected and all connection-related resources have been freed, so that a new connection can be attempted.|

## AnalogInput

This interface represents AnalogInput functionality, providing methods to obtain analog input readings and buffered samples.

`io = ioio.openAnalogInput(pin)`

| Name    | Description   |
|---------|---------------|
|float getVoltage(void)|Gets the analog input reading, as an absolute voltage in Volt units.|
|float getVoltageSync(void)|This is very similar to getVoltage(), but will wait for a new sample to arrive before returning.|
|float getReference(void)|Gets the maximum value against which read() values are scaled.|
|float read(void)|Gets the analog input reading, as a scaled real value between 0 and 1.|
|float readSync(void)|This is very similar to read(), but will wait for a new sample to arrive before returning.|
|int getOverflowCount(void)|Gets the number of samples that have been dropped as a result of overflow.|
|int available(void)|Gets the number of samples currently in the buffer. Reading that many samples is guaranteed not to block.|
|float readBuffered(void)|Read a sample from the internal buffer. This method will block until at least one sample is available.|
|float getVoltageBuffered(void)|Read a sample from the internal buffer. This method will block until at least one sample is available.|
|float getSampleRate(void)|Gets the sample rate used for obtaining buffered samples.|

## PulseInput

This interface represents PulseInput functionality, providing methods for pulse and frequency measurements.

`io = ioio.openPulseInput(pin)`

| Name    | Description   |
|---------|---------------|
|float getDuration(void)|Gets the pulse duration in case of pulse measurement mode, or the period in case of frequency mode.|
|float getDurationSync(void)|This is very similar to getDuration(), but will wait for a new sample to arrive before returning.|
|float getDurationBuffered(void)|Reads a single measurement from the queue. If the queue is empty, will block until more data arrives.|
|float getFrequency(void)|Gets the momentary frequency of the measured signal. When scaling is used, this is compensated for here.|
|float getFrequencySync(void)|This is very similar to getFrequency(), but will wait for a new sample to arrive before returning.|

## DigitalInput

This interface represents DigitalInput functionality, providing methods to read digital input pin values and wait for specific logical levels.

`io = ioio.openDigitalInput(pin)`

| Name    | Description   |
|---------|---------------|
|int read(void)|Read the value sensed on the pin. May block for a few milliseconds if called right after creation of the instance.|
|void waitForValue(int)|Block until a desired logical level is sensed. The calling thread can be interrupted for aborting this operation.|

## CapSense

This interface represents the CapSense functionality, allowing capacitance readings and threshold-based operations.

`io = ioio.openCapSense(pin)`

| Name    | Description   |
|---------|---------------|
|float read(void)|Gets the capacitance reading. It typically takes a few milliseconds.|
|float readSync(void)|This is very similar to read(), but will wait for a new sample to arrive before returning.|
|void setFilterCoef(float)|Sets the low-pass filter coefficient. This coefficient is the typical time constant of the system.|
|void waitOver(float)|Block until sensed capacitance becomes greater than a given threshold.|
|void waitOverSync(float)|This is very similar to waitOver(float), but will wait for a new sample to arrive before returning.|
|void waitUnder(float)|Block until sensed capacitance becomes less than a given threshold.|
|void waitUnderSync(float)|This is very similar to waitUnder(float), but will wait for a new sample to arrive before returning.|

## DigitalOutput

A pin used for digital output. A digital output pin can be used to generate logic-level signals. DigitalOutput instances are obtained by calling IOIO#openDigitalOutput. The value of the pin is set by calling write. The instance is alive since its creation. If the connection with the IOIO drops at any point, the instance transitions to a disconnected state, in which every attempt to use the pin (except close()) will throw a ConnectionLostException. Whenever close() is invoked the instance may no longer be used. Any resources associated with it are freed and can be reused. Typical usage:

`io = ioio.openDigitalOutput(pin)`

| Name    | Description   |
|---------|---------------|
|void write(int)|The output. true is logical \"HIGH\", false is logical \"LOW\".|

## PwmOutput

A pin used for PWM (Pulse-Width Modulation) output. A PWM pin produces a logic-level PWM signal. These signals are typically used for simulating analog outputs for controlling the intensity of LEDs, the rotation speed of motors, etc. They are also frequently used for controlling hobby servo motors. PwmOutput instances are obtained by calling IOIO#openPwmOutput. When used for motors and LEDs, a frequency of several KHz is typically used, where there is a trade-off between switching power-loses and smoothness of operation. The pulse width is typically set by specifying the duty cycle, with the setDutyCycle method. A duty cycle of 0 is \"off\", a duty cycle of 1 is \"on\", and every intermediate value produces an intermediate intensity. Please note that any devices consuming more than 20mA of current (e.g. motors) should not by directly connected the the IOIO pins, but rather through an amplification circuit suited for the specific load. When used for hobby servos, the PWM signal is rather used for encoding of the desired angle the motor should go to. By standard, a 100Hz signal is used and the pulse width is varied between 1ms and 2ms (corresponding to both extremes of the shaft angle), using setPulseWidth. The instance is alive since its creation. If the connection with the IOIO drops at any point, the instance transitions to a disconnected state, in which every attempt to use the pin (except close()) will throw a ConnectionLostException. Whenever close() is invoked the instance may no longer be used. Any resources associated with it are freed and can be reused. Typical usage (fading LED):

`io = ioio.openPwmOutput(pin, frequency)`

| Name    | Description   |
|---------|---------------|
|void setDutyCycle(float)|Sets the duty cycle of the PWM output. The duty cycle is defined to be the pulse width divided by the total cycle period. For absolute control of the pulse with, consider using setPulseWidth.|
|void setPulseWidth(float)|Sets the pulse width of the PWM output. The pulse width is duration of the high-time within a single period of the signal. For relative control of the pulse with, consider using setDutyCycle.|

## TwiMaster

An interface for controlling a TWI (Two Wire Interface) module, in TWI bus-master mode, enabling communication with multiple TWI-enabled slave modules.

`io = ioio.openTwiMaster(TWINumber, mode)`

| Name    | Description   |
|---------|---------------|
|void write(Address, Register, DataBytes)|Writes one or more bytes of data to the given register of an I2C device with given address.|
|int readWrite(address, NumReceiveBytes, Register, DataBytes)|Writes one or more bytes of data to the given register of an I2C device with given address and reads NumReceiveBytes. NumReceiveBytes can be max 8 bytes long.|

## SpiMaster

An interface for controlling an SPI (Serial Peripheral Interface) module, in SPI bus-master mode, enabling communication with multiple SPI-enabled slave modules

`io = ioio.openSpiMaster(misoPin, mosiPin, clkPin, csPin)`

| Name    | Description   |
|---------|---------------|
|void write(DataBytes)|Writes one or more bytes of data.|
|int readWrite(NumReceiveBytes, DataBytes)|Writes one or more bytes of data and reads NumReceiveBytes. NumReceiveBytes can be max 8 bytes long.|

