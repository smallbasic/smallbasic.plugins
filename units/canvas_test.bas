import canvas

c = canvas.create(120, 120, rgb(110, 110, 10))

for i = 0 to 120
  for j = 0 to 120
    c._pen = rgb(i+100, i*j % 255, j+100)
    canvas.draw_pixel(c, i, j)
  next
next

canvas.draw_rect_filled(c, 3, 3, 17, 17)

c._pen = rgb(210, 5, 255)
canvas.draw_circle(c, 50, 50, 34, true)

c._pen = rgb(10, 5, 25)
canvas.draw_circle(c, 50, 50, 35, false)

c._fontSize = 25
c._pen = rgb(255, 0,0)
canvas.draw_string(c, "abcdefghijklm", 2, 10)

c._fontSize = 19
c._pen = rgb(0, 255 ,0)
canvas.draw_string(c, "nopqrstuvwxyz", 2, 40)

c._fontSize = 1
c._pen = rgb(0, 0, 255)
canvas.draw_string(c, "1234567890", 2, 70)

png = image(c._dat)
png.save("foo.png")


