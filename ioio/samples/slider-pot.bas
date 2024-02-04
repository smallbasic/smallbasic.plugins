import ioio

rem
rem PIN CONNECTIONS:
rem GND -> GND
rem VCC -> 3.3v
rem OTA -> 46
rem

analogIn = ioio.openAnalogInput(46)
out = ioio.openDigitalOutput(0)

ioio.waitForConnect(10)

while 1
  n = analogIn.read()
  out.write(n > .5)
  print n
  delay 10
wend
