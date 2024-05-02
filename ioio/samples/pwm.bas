import ioio

led = ioio.openPwmOutput(46, 99)

print "wait for connect"
ioio.waitForConnect(10)
print "ready!!!"

for DutyCycle = 0 to 1 step 0.1
  led.setDutyCycle(DutyCycle)
  delay 100
next

print "Done"
