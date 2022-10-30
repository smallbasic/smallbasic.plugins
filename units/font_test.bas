import font1
import font2

for i = 50 to 200 step 40
  font1.drawtext(10, 100+i, i, "This is some text!")
next i

font2.drawtext(10, 20, 70, true, "()ABCDEFGHIJKLMNOPQRSTUVWXYZ *+,-./<>1234567890;")
font2.drawtext(10, 40, 100, true, "()ABCDEFGHIJKLMNOPQRSTUVWXYZ *+,-./<>1234567890;")
font2.drawtext(10, 60, 70, false, "()ABCDEFGHIJKLMNOPQRSTUVWXYZ *+,-./<>1234567890;")
font2.drawtext(10, 80, 100, false, "()ABCDEFGHIJKLMNOPQRSTUVWXYZ *+,-./<>1234567890;")

showpage
pause