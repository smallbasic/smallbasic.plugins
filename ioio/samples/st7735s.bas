rem
rem Educational BoosterPack MKII - Color TFT LCD Display
rem
rem The Crystalfontz CFAF128128B-0145T color 128x128-pixel TFT LCD supports display updates up to
rem 20 frames per second (FPS) while only requiring a few lines to control the TFT LCD module through
rem the SPI interface. This module has a color depth of 262K colors and a contrast ratio of 350.
rem
rem https://www.ti.com/tool/BOOSTXL-EDUMKII
rem https://www.ti.com/document-viewer/lit/html/SLAU599B#GUID-3E8385B7-69DD-4133-9660-C9C256762AA8/TITLE-SLAU599SLAU5992619
rem https://www.crystalfontz.com/product/cfaf128128b0145t-graphical-tft-128x128-lcd-display-module
rem https://www.crystalfontz.com/controllers/Sitronix/ST7735S/
rem https://github.com/crystalfontz/CFAF128128B1-0145T
rem https://github.com/crystalfontz/CFAF128128B1-0145T/blob/master/CFAF128128B1-0145T_SPI_Demo_Code/CFAF128128B1-0145T_SPI_Demo_Code.ino
rem

import ioio

const ST7735_SLPOUT 0x11
const ST7735_DISPOFF 0x28
const ST7735_DISPON 0x29
const ST7735_CASET 0x2A
const ST7735_RASET 0x2B
const ST7735_RAMWR 0x2C
const ST7735_RAMRD 0x2E
const ST7735_MADCTL 0x36
const ST7735_COLMOD 0x3A
const ST7735_FRMCTR1 0xB1
const ST7735_FRMCTR2 0xB2
const ST7735_FRMCTR3 0xB3
const ST7735_INVCTR 0xB4
const ST7735_PWCTR1 0xC0
const ST7735_PWCTR2 0xC1
const ST7735_PWCTR3 0xC2
const ST7735_PWCTR4 0xC3
const ST7735_PWCTR5 0xC4
const ST7735_VMCTR1 0xC5
const ST7735_GAMCTRP1 0xE0
const ST7735_GAMCTRN1 0xE1

rem J2.15 - LCD SPI MOSI
const mosiPin = 2

rem Unused - no data received from lcd
const misoPin = 6

rem J2.13 - LCD SPI chip select CS
rem Low: Controller chip is selected. Communications with host is possible.
rem High: Controller chip is not selected. Host interface signals are ignored by the controller.
const csPin = 3

' J1.7  - LCD SPI clock
const clkPin = 4

' J4.31 - LCD register select pin
const rsPin = 5

rem J4.17 - LCD reset pin
rem Low: Display controller is reset. The RST pin should be pulsed low shortly after power is applied.
rem High: The RST pin should be brought high (VDD) for normal operation.
const rstPin = 7

rem J4.39 - LCD backlight (Pin is multiplexed with the RGB LED red channel pin through the jumper header J5)
rem not implemented

const spi = ioio.openSpiMaster(misoPin, mosiPin, clkPin, csPin)
const rsOut = ioio.openDigitalOutput(rsPin)
const rstOut = ioio.openDigitalOutput(rstPin)

ioio.waitForConnect(10)

sub sendCommand(cmd)
  '  Select the LCD's command register
  rsOut.write(0)
  spi.write(cmd)
end

sub sendData(_data)
  rsOut.write(1)
  spi.write(_data)
end

sub resetDisplay()
  rstOut.write(0)
  delay 50
  rstOut.write(1)
  delay 150
end

sub initST7735S()
  resetDisplay()
  sendCommand(0x01)  ' Software reset
  delay 150
  sendCommand(ST7735_SLPOUT)
  delay 255
  sendCommand(ST7735_DISPON)
end

sub set_LCD_for_write_at_X_Y(x, y)
  rem CASET (2Ah): Column Address Set
  rem  * The value of XS [15:0] and XE [15:0] are referred when RAMWR
  rem    command comes.
  rem  * Each value represents one column line in the Frame Memory.
  rem  * XS [15:0] always must be equal to or less than XE [15:0]
  sendCommand(ST7735_CASET) ' Column address set
  rem Write the parameters for the "column address set" command
  'sendData(0x00)     ' Start MSB = XS[15:8]
  'sendData(0x02 + x) ' Start LSB = XS[ 7:0]
  'sendData(0x00)     ' End MSB   = XE[15:8]
  'sendData(0x81)     ' End LSB   = XE[ 7:0]
  local _data = [0x00, 0x02 + x, 0x00, 0x81]
  sendData(_data)

  rem Write the "row address set" command to the LCD
  rem RASET (2Bh): Row Address Set
  rem  * The value of YS [15:0] and YE [15:0] are referred when RAMWR
  rem    command comes.
  rem  * Each value represents one row line in the Frame Memory.
  rem  * YS [15:0] always must be equal to or less than YE [15:0]
  sendCommand(ST7735_RASET) ' Row address set

  rem Write the parameters for the "row address set" command
  'sendData(0x00)     ' Start MSB = YS[15:8]
  'sendData(0x01 + y) ' Start LSB = YS[ 7:0]
  'sendData(0x00)     ' End MSB   = YE[15:8]
  'sendData(0x80)     ' End LSB   = YE[ 7:0]
  _data = [0x00, 0x01 + y, 0x00, 0x80]
  sendData(_data)

  rem Write the "write data" command to the LCD
  rem RAMWR (2Ch): Memory Write
  sendCommand(ST7735_RAMWR) ' write data
end

rem Fill display with a given RGB value
sub fill_LCD(r, g, b)
  local i, _data
  ' higher values didn't improve performance
  local bufSize = 40

  for i = 1 to bufSize
    _data << b
    _data << g
    _data << r
  next

  Set_LCD_for_write_at_X_Y(0, 0)
  rsOut.write(1)
  for i = 0 to (128 * 128) / bufSize
    spi.write(_data)
  next i
end

rem/Write the single pixel's worth of data
sub put_Pixel(x, y, r, g, b)
  Set_LCD_for_write_at_X_Y(x, y)
  sendData([b,g,r])
end

initST7735S()
randomize
t1 = timer
fill_LCD(rnd*255, rnd*255, rnd*255)
print format("Elap: ###", timer-t1)
delay 5000
