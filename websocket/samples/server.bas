import websocket as ws

print "websocket server"
conn = ws.listen(8000, 1)
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print msg
  endif
  delay 1000
wend

