import ioio

const mosiPin = 2
const misoPin = 6
const csPin = 3
const clkPin = 4

spi = ioio.openSpiMaster(misoPin, mosiPin, sckPin, csPin)
cs = ioio.openDigitalOutput(csPin)

ioio.waitForConnect(10)

rem true sets the initial state to high (deasserted)
cs.write(true)

spi.write(0x09, 0x00)  ' Decode mode: no decode for digits 0-7
spi.write(0x0A, 0x07)  ' Intensity: maximum intensity 0x0 -> 0x0F
spi.write(0x0B, 0x07)  ' Scan limit: all digits
spi.write(0x0C, 0x01)  ' Shutdown: normal operation

while 1
  for i = 1 to 8
    spi.write(0x55)
  next
  delay 1000
wend
'
