import ioio

out = ioio.openDigitalOutput(0)
value = false
while (true) 
  print "setting LED "+ value
  out.write(value)
  value = !value
  delay 1000
wend  



