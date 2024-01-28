import ioio

analogIn = ioio.openAnalogInput(46)

print "wait for connect"
ioio.waitForConnect()
print "ready!!!"

while 1
  print analogIn.read()
  delay 100
wend

print "done"



