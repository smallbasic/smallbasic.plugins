import ioio

out = ioio.openDigitalOutput(0)
while !out.isReady()
  delay 1000
wend

value = false
for i = 0 to 5
  out.write(value)
  value = !value
  delay 100
next  

for i = 0 to 5
  out.write(value)
  value = !value
  delay 10
next

print "done"



