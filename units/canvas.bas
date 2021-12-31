unit canvas

export create
export draw_line
export draw_pixel
export draw_char
export draw_circle
export draw_line
export draw_rect
export draw_rect_filled
export draw_string

const font = load_font("font7x4.json")

sub iterate(w, h, filter)
  local x, y
  for y = 0 to h - 1
    for x = 0 to w - 1
      call filter, x, y
    next
  next
end

func load_font(name)
  local font = {}
  local col = 0
  local row = 0
  local ch, txt, buffer
  local w = 4
  local h = 7

  tload name, txt, 1
  buffer = array(txt)

  func slice
    local result
    dim result(h, w)
    sub do_slice(x, y)
      result[y, x] = buffer[y + row, x + col]
    end
    iterate(w, h, @do_slice)
    return result
  end

  for ch = asc("a") to asc("j")
    c = chr(ch)
    font[c] = slice()
    col += w + 1
  next

  row += h + 1
  col = 0
  for ch = asc("k") to asc("t")
    c = chr(ch)
    font[c] = slice()
    col += w + 1
  next

  row += h + 1
  col = 0
  for ch = asc("u") to asc("z")
    c = chr(ch)
    font[c] = slice()
    col += w + 1
  next

  row += h + 1
  row += h + 1
  col = 0
  for ch = asc("0") to asc("9")
    c = chr(ch)
    font[c] = slice()
    col += w + 1
  next

  col = 30
  row = 16
  font[" "] = slice()

  col += w + 1
  font["."] = slice()

  col += w + 1
  font[","] = slice()

  col += w + 1
  font["!"] = slice()

  return font
end

sub draw_pixel(byref canvas, x1, y1)
  if (x1 > -1 and y1 > -1 and y1 <= ubound(canvas._dat, 1) and x1 <= ubound(canvas._dat, 2)) then
    canvas._dat[y1, x1] = canvas._pen
  endif
end

func draw_char(byref canvas, c, x0, y0)
  local glyph = font[lower(c)]
  local g_height = ubound(glyph, 1)
  local g_width = ubound(glyph, 2)
  local c_height = canvas._fontSize
  local c_width = int(c_height * .55)
  local yscale = c_height / g_height
  local xscale = c_width / g_height

  sub filter(x, y)
    local x1 = round(x / xscale)
    local y1 = round(y / yscale)
    if (y0 + y <= canvas._h and x0 + x <= canvas._w and y1 <= g_height and x1 <= g_width and glyph[y1, x1] != 0) then
      canvas._dat[y0 + y, x0 + x] = canvas._pen
    endif
  end

  sub transfer(x, y)
    if (glyph[y, x] != 0) then
      canvas._dat[y0 + y, x0 + x] = canvas._pen
    endif
  end

  if (canvas._fontSize < 19) then
    iterate(g_width, g_height, @transfer)
  else
    iterate(c_width, c_height, @filter)
  endif

  return [g_height, g_width]
end

sub draw_string(byref canvas, s, x0, y0)
  local x = x0
  local y = y0
  local c, m, spacing
  for c in s
    m = draw_char(canvas, c, x, y)
    spacing = m[1] / m[0] * canvas._fontSize * .35
    x += m[1] + spacing
  next
end

sub draw_line(byref canvas, x0, y0, x1, y1)
  local dx =  abs(x1 - x0)
  local dy = -abs(y1 - y0)
  local sx =  iff(x0 < x1, 1, -1)
  local sy =  iff(y0 < y1, 1, -1)
  local err = dx + dy
  local e2

  while 1
    draw_pixel(canvas, x0, y0)
    if (x0 == x1 and y0 == y1) then exit loop
    e2 = err * 2
    if (e2 >= dy) then
      err += dy
      x0 += sx
    endif
    if (e2 <= dx) then
      err += dx
      y0 += sy
    endif
  wend
end

sub draw_rect(byref canvas, x0, y0, x1, y1)
  draw_line(canvas, x0, y0, x1, y0)
  draw_line(canvas, x0, y1, x1, y1)
  draw_line(canvas, x0, y0, x0, y1)
  draw_line(canvas, x1, y0, x1, y1)
end

sub draw_rect_filled(byref canvas, x0, y0, x1, y1)
  local x, y
  for y = max(y0, 0) to min(y1, canvas._h)
    for x = max(x0, 0) to min(x1, canvas._w)
      canvas._dat[y, x] = canvas._pen
    next
  next
end

sub draw_circle(byref canvas, x0, y0, r, as_filled)
  sub fill_circle(x, y, r)
    draw_line canvas, -x + x0,  y + y0, x + x0,  y + y0
    draw_line canvas, -x + x0, -y + y0, x + x0, -y + y0
    draw_line canvas, -y + x0,  x + y0, y + x0,  x + y0
    draw_line canvas, -y + x0, -x + y0, y + x0, -x + y0
  end

  sub plot_circle(x, y, r)
    ' left + right mid quads
    draw_pixel canvas,  x + x0,  y + y0
    draw_pixel canvas, -x + x0,  y + y0
    draw_pixel canvas,  x + x0, -y + y0
    draw_pixel canvas, -x + x0, -y + y0
    ' left + right top/bottom quads
    draw_pixel canvas,  y + x0, x + y0
    draw_pixel canvas, -y + x0, x + y0
    draw_pixel canvas,  y + x0, -x + y0
    draw_pixel canvas, -y + x0, -x + y0
  end

  local painter
  if (as_filled) then
    painter = @fill_circle
  else
    painter = @plot_circle
  endif

  local x = r
  local y = 0
  local p = 1 - r

  call painter, x, y, r
  while (x > y)
    y++
    if (p <= 0) then
      ' Mid-point is inside or on the perimeter
      p += (2 * y) + 1
    else
      ' Mid-point is outside the perimeter
      x--
      p += (2 * y) - (2 * x) + 1
    endif

    ' All the perimeter points have already been printed
    if (x < y) then exit loop
    call painter, x, y, r
  wend
end

func create(width, height, _pen)
  local result = {}
  dim result._dat(height, width)
  result._pen = _pen
  result._fontSize = 20
  result._w = width
  result._h = height
  return result
end
