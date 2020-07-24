import websocket as ws

conn = ws.create("ws://127.0.0.1:8000", "ws_chat")

while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print msg
    ws.send(conn, "hello")
  endif
  delay 1000
wend

ws.close(conn)

