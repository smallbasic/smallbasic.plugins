rem
rem Hardware Setup:
rem Power Supply: Connect VCC of the 1088AS to a suitable power supply voltage (usually 5V).
rem Ground Connection: Connect GND of the 1088AS to the ground (GND) of the IOIO-OTG board.
rem Data Connections:
rem Connect the DIN pin of the 1088AS to a digital output pin on the IOIO-OTG board.
rem Connect the CS pin of the 1088AS to another digital output pin on the IOIO-OTG board.
rem Connect the CLK pin of the 1088AS to yet another digital output pin on the IOIO-OTG board.
rem
rem In the context of an LED matrix display like the 8x8 Duinotech 1088AS,
rem writing to the DIN (Data Input) pin is not directly addressing a
rem particular LED. Instead, it's part of a process to send data to the
rem display to control which LEDs are lit up.
rem 
rem Here's a simplified overview of how it works:
rem 
rem Data Shifted In: You send a stream of data serially to the
rem display. Each bit of data represents whether a corresponding LED
rem should be on or off.
rem 
rem Data Shifted Out to LEDs: As the data is shifted into the display
rem through the DIN pin, it is internally stored in shift registers. When
rem enough data is received (for example, 8 bits for an 8x8 LED matrix),
rem the display will update the LEDs accordingly.
rem 
rem Control Signals: Alongside the data, you typically send control
rem signals like clock pulses (CLK) to synchronize the shifting of data
rem and chip select (CS) to indicate when the data is valid.
rem 
rem Matrix Multiplexing: The LED matrix is typically arranged in rows and
rem columns. By controlling the state of each row and column (using
rem additional control pins or internally within the display), you can
rem select individual LEDs or groups of LEDs to turn on or off.
rem 
rem Updating Display: After sending the necessary data and control
rem signals, the display refreshes itself to reflect the new state of the
rem LEDs based on the received data.
rem 
rem So, when you write to the DIN pin, you're not directly addressing a
rem particular LED. Instead, you're sending data to the display to update
rem the entire matrix, and by controlling the row and column pins, you
rem indirectly address specific LEDs or groups of LEDs within the matrix.
rem

import ioio

const dinPin = 2
const csPin = 3
const clkPin = 4

din = ioio.openDigitalOutput(dinPin)
cs = ioio.openDigitalOutput(csPin)
clk = ioio.openDigitalOutput(clkPin)

ioio.waitForConnect(10)

while 1
  shiftOut(0x55) 
  delay 10000
wend

sub shiftOut(_data)
  cs.write(false); ' Enable the chip
  for i = 7 to 0 step -1
    clk.write(false) ' Start clock pulse
    din.write((_data band (1 lshift i)) != 0) ' Send bit
    clk.write(true) ' End clock pulse
  next
  cs.write(true) ' Disable the chip
end
