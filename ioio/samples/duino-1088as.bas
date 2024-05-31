import ioio

' mosi = master-in slave-out (miso is unused here)
const mosiPin = 2
const misoPin = 6
const csPin = 3
const clkPin = 4

spi = ioio.openSpiMaster(misoPin, mosiPin, clkPin, csPin)
ioio.waitForConnect(10)

spi.write(0x09, 0x00)  ' Decode mode: no decode for digits 0-7
spi.write(0x0A, 0x00)  ' Intensity: maximum intensity 0x0 -> 0x0F
spi.write(0x0B, 0x07)  ' Scan limit: all digits
spi.write(0x0C, 0x01)  ' Shutdown: normal operation

for i = 1 to 8
  spi.write(i, 0)
next

const glyph_a = [
  [0,0,0,0,1,0,0,0],
  [0,0,0,1,0,1,0,0],
  [0,0,1,0,0,0,1,0],
  [0,0,1,0,0,0,1,0],
  [0,0,1,1,1,1,1,0],
  [0,1,0,0,0,0,0,1],
  [0,1,0,0,0,0,0,1],
  [0,0,0,0,0,0,0,0],
]

const glyph_b = [
  [0,1,0,0,0,0,0,0],
  [0,1,0,0,0,0,0,0],
  [0,1,0,0,0,0,0,0],
  [0,1,1,1,0,0,0,0],
  [0,1,0,0,1,0,0,0],
  [0,1,0,0,0,1,0,0],
  [0,1,0,0,0,1,0,0],
  [0,0,1,1,1,0,0,0],
]

sub print_glyph(byref f)
  local i, k, n
  for i = 0 to 7
    n = 0
    for k = 0 to 7
      if (f[i][k] == 1) then
        n += pow(2, 7 - k)
      endif
    next k
    spi.write(i + 1, n)
  next i
end

while 1
  print_glyph(glyph_a)
  delay 1000
  print_glyph(glyph_b)
  delay 1000
wend  
