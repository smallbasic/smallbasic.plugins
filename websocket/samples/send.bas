import websocket as ws

print "websocket server"
conn = ws.listen("http://localhost:8000", 1)
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print msg
  endif
  ws.send(conn, time)
  delay 10
wend

