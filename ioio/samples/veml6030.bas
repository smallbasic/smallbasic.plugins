rem
rem https://learn.sparkfun.com/tutorials/qwiic-ambient-light-sensor-veml6030-hookup-guide/all
rem https://piico.dev/p3
rem https://www.vishay.com/docs/84366/veml6030.pdf
rem PiicoDev Ambient Light Sensor VEML6030
rem

import ioio

rem
rem wiring:
rem SDA -> pin 1
rem CLK -> pin 2
rem

rem i2c address
const address = 0x10

rem register where the light sensing data is stored
const alsDataReg = 0x04

rem measure the total brightness of the ambient light regardless of its color
const whiteDataReg = 0x05

rem ambient light sensing configuration register
const alsConfReg = 0

rem default settings
rem initialise gain:1x, integration 100ms, persistence 1, disable interrupt
const alsConf = 0

p3 = ioio.openTwiMaster(1, 0)

ioio.waitForConnect(10)

rem configure default settings
p3.write(address, [alsConfReg, alsConf])

for i = 0 to 5
  print p3.readWrite(address, 2, [alsDataReg])
  delay 1000
next
