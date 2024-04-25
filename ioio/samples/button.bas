' PUSH BUTTON
' ===========
'
' This example demonstrates how to connect a push button
' and read the state of this button. If the button is pressed
' a 0 will be returned otherwise 1
'
'            ---------------
' PIN10 o----| Push Button |----o GND  
'            ---------------
'
' The push button is connected to pin 10 and to GND of the IOIO board.


import ioio

PIN10 = ioio.openDigitalInput(10)


print "Wait for connection to IOIO board"
ioio.waitForConnect(10)
print "Connection established"
print 
print "Press button connected to IOIO board or q for quit"

isRunning = true

while(isRunning)

    key = inkey()
    if(key == "q") then isRunning = false
    
    value = PIN10.read()
    
    locate(6,0): print "Button value is: " + value
    
    delay(50)
    
wend

print "done"
