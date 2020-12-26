import websocket as ws

print "websocket server"
conn = ws.listen("http://localhost:8000/")
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (ismap(msg)) then
    print msg.id + ":" + msg.data
    ws.send(conn, msg.data, msg.id)
  endif
  delay 10
wend
