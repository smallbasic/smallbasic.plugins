' BH1750 - Ambient light sensor
' =============================
'
' This examample demonstrates how to measure the ambient light luminous flux
' using the BH1750 I2C sensor.
' 
' Connect the sensor to the IOIO-OTG board:
'
' ------         ------
'  IOIO |       |BH1750
'  PIN 4|-------|SDA 
'  PIN 5|-------|SCL
'  GND  |-------|GND
'  3.3V |-------|VIN
'       |       |ADDR
'-------         ------

' If ADDR is not connected, 0x23 as I2C address will be used.
' 

import ioio

const ADDRESS = 0x23

Print "Connect to BH1750"
sensor = ioio.openTwiMaster(0, 0)
ioio.waitForConnect(10)
Print "Connection established"

' Power down
sensor.write(ADDRESS, 0x00)
' Power on
sensor.write(ADDRESS, 0x01)
delay(1000)

' Read one time with low resolution
ValueLowRes = sensor.readwrite(ADDRESS, 2, 0x23) / 1.2
delay(1000)
' Read one time with high resolution
ValueHighRes = sensor.readwrite(ADDRESS, 2, 0x20) / 1.2

print "Low resolution : " + ValueLowRes  + " lx"
print "High resolution: " + valueHighRes + " lx"
    

    