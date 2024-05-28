' ST7789 - TFT controller
' =============================
'
' This example demonstrates how to drive a TFT display with a ST7789 controller.
' With the current IOIO implementation the example works but is unusable slow.
'                                          ----------------------------------
' There exist many TFT displays using the ST7789 controller. This examples is written
' for the Waveshare 1.3inch LCD module with 240x240 pixels. With some minor modification
' especially for the pins, TFTs from other manufacturers should also work. Be carefull
' with VCC. If you are using a bare TFT, then drive it only with 3.3V. Many breakout
' boards (i.e. Adafruit) support 5V.
'
' -------         ------
'  IOIO  |       |TFT
'  PIN 40|-------|DIN (MOSI) 
'  PIN 39|-------|CLK (SCL)
'  PIN 38|-------|CS
'  PIN 37|-------|DC
'  PIN 36|-------|RST
'  PIN 35|-------|BL
'  GND   |-------|GND
'  5V    |-------|VIN
'--------         ------

' This example is based on the C library for Arduino:
' https://github.com/cbm80amiga/Arduino_ST7789_Fast/blob/master/Arduino_ST7789_Fast.cpp
' -------------------------------------------------------------------------------------

import ioio

const MISO = 34      ' SPI MISO (unused)
const DIN  = 40      ' SPI MOSI
const CLK  = 39      ' SPI clock
const CS   = 38      ' SPI Chip select
const DC   = 37      ' Data or command -> HIGH = data / LOW = command 
const RST  = 36      ' Chip reset
const BL   = 35      ' Blacklight control

const ST7789_NOP            = 0x00
const ST7789_SWRESET        = 0x01
const ST7789_SLPOUT         = 0x11
const ST7789_NORON          = 0x13
const ST7789_INVON          = 0x21
const ST7789_DISPON         = 0x29
const ST7789_CASET          = 0x2A
const ST7789_RASET          = 0x2B
const ST7789_RAMWR          = 0x2C
const ST7789_COLMOD         = 0x3A
const ST7789_MADCTL         = 0x36
const ST7789_MADCTL_MY      = 0x80
const ST7789_MADCTL_MX      = 0x40
const ST7789_MADCTL_MV      = 0x20
const ST7789_MADCTL_ML      = 0x10
const ST7789_MADCTL_RGB     = 0x00
const ST7789_240x240_XSTART = 0
const ST7789_240x240_YSTART = 0
const ST7789_TFTWIDTH       = 240
const ST7789_TFTHEIGHT      = 240

const BLACK   = 0x0000
const BLUE    = 0x001F
const RED     = 0xF800
const GREEN   = 0x07E0
const CYAN    = 0x07FF
const MAGENTA = 0xF81F
const YELLOW  = 0xFFE0
const WHITE   = 0xFFFF

const HIGH      = TRUE
const LOW       = FALSE
const PIN_DELAY = 1

colstart = 0
rowstart = 0
ystart   = 0
xstart   = 0
width    = 240
height   = 240

Setup(240, 240)             ' parameter: TFT width , TFT height
FillScreen(GREEN)

'for xx = 100 to 150
'  DrawPixel(xx, 100, RGBto565(255, 0, 255))
'next

print "done"

'########################################

sub Setup(w, h)
    Print "Connect to TFT"
    SPI      = ioio.openSpiMaster(MISO, DIN, CLK, CS)
    ResetPin = ioio.openDigitalOutput(RST) 
    DCPin    = ioio.openDigitalOutput(DC)
    BLPin    = ioio.openDigitalOutput(BL)
    ioio.waitForConnect(10)
    Print "Connection established"

    if(w == 240 and h == 240) then rowstart = 80
    width = w
    height = h

    ' Background light on
    BLPin.write(HIGH)

    ' Hardware reset
    ResetPin.write(HIGH)
    delay(50)
    ResetPin.write(LOW)
    delay(50)
    ResetPin.write(HIGH)
    delay(150)

    'Init
    writeCmd(ST7789_SWRESET) : delay(150)
    writeCmd(ST7789_SLPOUT)  : delay(500)
    writeCmd(ST7789_COLMOD)  : writeData8(0x55) : delay(10)     ' RGB565
    writeCmd(ST7789_MADCTL)  : writeData8(0x00)
    writeCmd(ST7789_CASET)   : writeData16(ST7789_240x240_XSTART) : writeData16(ST7789_TFTWIDTH + ST7789_240x240_XSTART)
    writeCmd(ST7789_RASET)   : writeData16(ST7789_240x240_YSTART) : writeData16(ST7789_TFTHEIGHT + ST7789_240x240_YSTART)
    writeCmd(ST7789_INVON)   : delay(10)
    writeCmd(ST7789_NORON)   : delay(10)
    writeCmd(ST7789_DISPON)  : delay(10)

    SetRotation(2)
end

func RGBto565(r,g,b)
    return ((((r) BAND 0xF8) lshift 8) BOR (((g) BAND 0xFC) lshift 3) BOR ((b) rshift 3)) 
end

sub WriteCmd( c) 
  DCPin.write(LOW)
  delay(PIN_DELAY)
  SPI.write(c, ST7789_NOP)
end

sub WriteData8(Data_Uint8)
  DCPin.write(HIGH)
  delay(PIN_DELAY)
  SPI.write(Data_Uint8, 0)
end

sub WriteData16(Data_Uint16)
  DCPin.write(HIGH)
  delay(PIN_DELAY)
  SPI.write(Data_Uint16 rshift 8, Data_Uint16 BAND 0xFF)
end

sub DrawPixel(x, y, c)
  setAddrWindow(x, y, x + 1, y + 1)  
  WriteCmd(ST7789_RAMWR)
  writeData16(c)
end

sub FillRect(x, y, w, h, col)
  if(x >= width OR y >= height OR w <= 0 OR h <= 0) then return
  if(x + w - 1 >= width) then w = width - x
  if(y + h - 1 >= height) then h = height - y
  
  setAddrWindow(x, y, x + w - 1, y + h - 1) 
  WriteCmd(ST7789_RAMWR)
  
  DCPin.write(HIGH)
  delay(PIN_DELAY)
  num = w * h
  c_high = col rshift 8
  c_low  = col BAND 0xFF
  while(num)
    num--
    SPI.write(c_high, c_low)
  wend
end

sub FillScreen(col) 
  FillRect(0, 0,  width, height, col)
end

sub SetRotation(m) 
  writeCmd(ST7789_MADCTL)
  rotation = m BAND 3
  select case rotation
    case 0
      writeData8(ST7789_MADCTL_MX BOR ST7789_MADCTL_MY BOR ST7789_MADCTL_RGB)
      xstart = colstart
      ystart = rowstart
    case 1
      writeData8(ST7789_MADCTL_MY BOR ST7789_MADCTL_MV BOR ST7789_MADCTL_RGB)
      ystart = colstart
      xstart = rowstart
    case 2
      writeData8(ST7789_MADCTL_RGB)
      xstart = 0
      ystart = 0
    case 3
      writeData8(ST7789_MADCTL_MX BOR ST7789_MADCTL_MV BOR ST7789_MADCTL_RGB)
      xstart = 0
      ystart = 0
  end select
end


sub setAddrWindow(xs, xe, ys, ye)
  xs += xstart
  xe += xstart
  ys += ystart
  ye += ystart

  'CASET
  WriteCmd(ST77XX_CASET)  
  DCPin.write(HIGH)                                  ' data (active high)
  SPI.write(xs rshift 8, xs BAND 0xFF)
  SPI.write(xe rshift 8, xe BAND 0xFF)   
  ' RASET
  WriteCmd(ST77XX_RASET)
  DCPin.write(HIGH)                                  ' data (active high)
  SPI.write(ys rshift 8, ys BAND 0xFF)
  SPI.write(ye rshift 8, ye BAND 0xFF)
end
