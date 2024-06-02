' SSD1306 - I2C OLED TFT with 128x64 monochrome pixels
' ===============================================
'
' This example demonstrates how to draw graphics on a
' SSD1306 OLED display connected via I2C
' 
' Connect the display to the IOIO-OTG board:
'
' ------         ------
'  IOIO |       |SSD1306
'  PIN 4|-------|SDA 
'  PIN 5|-------|SCL
'  GND  |-------|GND
'  3.3V |-------|VCC
'-------         ------

import ioio         ' module for IOIO Board
import canvas       ' unit to draw graphics in a framebuffer

const ADDRESS = 0x3C
const WIDTH   = 128
const HEIGHT  = 64

Print "Connect to SSD1306 OLED display"
oled = ioio.openTwiMaster(0, 0)
ioio.waitForConnect(10)
Print "Connection established"

Init_ssd1306()

' Init canvas
c = canvas.create(WIDTH, HEIGHT, 1)
c._fontSize = 7

' Draw some graphics
c._pen = 0                                   ' Set draw color to 0
canvas.draw_rect_filled(c, 0,0,WIDTH,HEIGHT) ' Clear screen
c._pen = 1                                   ' Set draw color to 1
canvas.draw_circle(c, 25, 40, 16, true)
canvas.draw_string(c, "IOIO with", 60, 0)
canvas.draw_string(c, "SMALLBASIC", 60, 15)
canvas.draw_line(c, 0, 0, 127, 63)
canvas.draw_line(c, 0, 0, 0, 63)
canvas.draw_line(c, 0, 63, 127, 63)

' Transfer framebuffer to display
TransferFramebuffer(c._dat)         ' c._dat is the canvas framebuffer
delay(1000)

' Set brightness
SetBrightness(20)
delay(1000)
SetBrightness(255)
delay(1000)

' Display on/off
DisplayOff()
delay(1000)
DisplayOn()
delay(1000)

' Set inverse
SetInverse(TRUE)
delay(1000)
SetInverse(FALSE)

print "done"

'##############################################

sub SendCommand(c)
    oled.write(ADDRESS, 0, c)
end

sub Init_ssd1306()
    ' Init sequence according to specsheet
    ' Display off
    SendCommand(0xAE)
    ' Set Multiplex Ratio
    SendCommand(0xA8) 
    SendCommand(SSD1306_LCDHEIGHT - 1)
    ' Display Offset
    SendCommand(0xD3)
    SendCommand(0x0)
    ' Set Display Clock Divide Ratio / Oscillator Frequency; suggested ratio 0x80
    SendCommand(0xD5)
    SendCommand(0x80)
    ' Display Start Line -> 0
    SendCommand(0x40)
    ' Set Segment Re-map: column address 127 is mapped to SEG0
    SendCommand(0xA1)
    ' Set COM Output Scan Direction: remapped mode. Scan from COM[N-1] to COM0
    SendCommand(0xC8)
    ' Set COM Pins Hardware Configuration
    ' Alternative COM pin configuration + Disable COM Left/Right remap
    SendCommand(0xDA)
    SendCommand(0x12)
    ' Set Contrast Control (Brightness)
    ' 0 to 255
    SendCommand(0x81)
    SendCommand(200)
    ' Entire Display ON: A4h command enable display outputs according to the GDDRAM contents
    SendCommand(0xA4)
    ' Set Normal Display: This command sets the display to be either normal or inverse.
    ' In normal display a RAM data of 1 indicates an  “ON” pixel while in inverse display
    ' a RAM data of 0 indicates an “ON” pixel
    ' 0xA6 normal, 0xA7 inverse
    SendCommand(0xA6)   ' <- for testing set to inverse. The display should be white. 
    ' Charge Pump Setting
    ' Enable Charge Pump
    SendCommand(0x8D)
    SendCommand(0x14)
    ' Memory Addressing Mode
    ' Horizontal Addressing Mode
    SendCommand(0x20)
    SendCommand(0x00)
    ' turn on display
    SendCommand(0xAF)
end

sub TransferFramebuffer(byref fb)
    local ii, xx, yy, aa, chunk, t, FRAMEBUFFER_SIZE, FrameBuffer_1bit
    
    FRAMEBUFFER_SIZE = WIDTH * HEIGHT / 8
    dim FrameBuffer_1bit(FRAMEBUFFER_SIZE)
    
    ' The display is monochrome. A on-pixel is 1, a off-pixel is 0.
    ' In display RAM every byte stores 8 pixels. The canvas framebuffer
    ' needs to be rearranged.
    ii = 0
    for xx = 0 to HEIGHT - 8 Step 8
        for yy = 0 to WIDTH - 1
            FrameBuffer_1bit[ii] = (fb[xx+7,yy] lshift 7) BOR (fb[xx+6,yy] lshift 6) BOR (fb[xx+5,yy] lshift 5) BOR (fb[xx+4,yy] lshift 4) BOR (fb[xx+3,yy] lshift 3) BOR (fb[xx+2,yy] lshift 2) BOR (fb[xx+1,yy] lshift 1) BOR fb[xx,yy]
            ii++
        next
    next
    
    ' The framebuffer can't be send in one go (restriction by IOIO).
    ' The framebuffer is cut in chunks each 33 bytes long. Every
    ' chunk is transfered sequentially. The first element of every
    ' chunk is the command 0x40 for writing to display RAM. The
    ' following 32 bytes are framebuffer data.
    dim chunk[32]

    for ii = 0 to FRAMEBUFFER_SIZE - 32 step 32
      for aa = 1 to 32
        chunk[0] = 0x40
        chunk[aa] = FrameBuffer_1bit[ii + aa - 1]
      next
      oled.write(ADDRESS, chunk)
    next
end

sub SetBrightness(b)
    ' Set Contrast Control (Brightness)
    ' 0 to 255
    if(b < 0) then b = 0
    if(b > 255) then b = 255
    SendCommand(0x81)
    SendCommand(b)
end

sub DisplayOn()
    SendCommand(0xAF)
end

sub DisplayOff()
    SendCommand(0xAE)
end

sub SetInverse(i)
    if(i) then
        SendCommand(0xA7)
    else
        SendCommand(0xA6)
    endif
end
