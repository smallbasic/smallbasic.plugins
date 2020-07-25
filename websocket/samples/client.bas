import websocket as ws

conn = ws.create("ws://127.0.0.1:8000", "ws_chat")
print "conn = " + conn
i = 0
while ws.open(conn) == 1 && i < 5
  i++
  ws.send(conn, "hello: " + i)
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print "message = " + msg
  endif
  delay 1000
wend

ws.close(conn)

