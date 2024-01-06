import ioio

out = ioio.openDigitalOutput(0)
'delay 30000
value = false
for i = 0 to 5
  print "setting LED "+ value
  out.write(value)
  value = !value
  delay 1000
next  
print "done"



