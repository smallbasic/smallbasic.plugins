import websocket as ws

conn = ws.create("ws://127.0.0.1:8000", "ws_chat")
print "conn = " + conn
while ws.open(conn) == 1
  ws.send(conn, "hello")
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print "message = " + msg
  endif
  delay 1000
wend

ws.close(conn)

