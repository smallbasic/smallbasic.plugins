import websocket as ws

print "websocket server"
conn = ws.listen("http://localhost:8000/")
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print msg
  endif
  delay 10
  ws.send(conn, time)
wend

