import websocket as ws

conn = ws.create("ws://127.0.0.1:8000", "ws_chat")
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print "Received:" + msg
    input k
    if (len(k) == 0) then exit loop
    ws.send(conn, k)
  endif  
  delay 1000
wend

ws.close(conn)

