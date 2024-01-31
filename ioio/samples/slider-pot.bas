import ioio

analogIn = ioio.openAnalogInput(46)
ioio.waitForConnect(10)

while 1
  print analogIn.read()
  delay 10
wend
