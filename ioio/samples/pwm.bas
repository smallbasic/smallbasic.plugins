' PWM with a LED
' =============================
'
' This example demonstrates how to use PWN to
' control the brightness of a LED
' 
' Connect a LED and a resistor as following:
'
'            |\ |
'  IOIO      | \|    --------     IOIO
'  PIN 46 ---|  |---| R = 1k |--- GND
'            | /|    --------
'            |/ |

import ioio

led = ioio.openPwmOutput(46, 1000)

print "wait for connect"
ioio.waitForConnect(10)
print "ready!!!"

for DutyCycle = 0 to 1 step 0.01
  led.setDutyCycle(DutyCycle)
  delay 10
next

print "Done"
