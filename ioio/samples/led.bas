import ioio

out = ioio.openDigitalOutput(13)

'print "wait for connect"
ioio.waitForConnect()
'print "ready!!!"

value = false
for i = 0 to 5
  out.write(value)
  value = !value
  delay 2000
next  

for i = 0 to 5
  out.write(value)
  value = !value
  delay 3000
next

'print "done"
