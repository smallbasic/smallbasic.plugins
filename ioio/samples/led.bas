import ioio

out = ioio.openDigitalOutput(0)

print "wait for connect"
ioio.waitForConnect(10)
print "ready!!!"

value = false
for i = 0 to 5
  out.write(value)
  value = !value
  delay 1000
next  

for i = 0 to 5
  out.write(value)
  value = !value
  delay 100
next

print "done"
