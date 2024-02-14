rem
rem https://learn.sparkfun.com/tutorials/qwiic-ambient-light-sensor-veml6030-hookup-guide/all
rem https://piico.dev/p3
rem PiicoDev Ambient Light Sensor VEML6030
rem

import ioio

rem i2c address
const address = 0x10

rem register where the light sensing data is stored
const alsDataReg = 0x04

rem ambient light sensing configuration register
const alsConfReg = 0

rem default settings
rem initialise gain:1x, integration 100ms, persistence 1, disable interrupt
const alsConf = 0

p3 = ioio.openTwiMaster(1, 0)

ioio.waitForConnect(10)

rem configure default settings
p3.write(address, alsConfReg, alsConf)

while 1
  print p3.readWrite(address, alsDataReg)
  delay 2000
wend
