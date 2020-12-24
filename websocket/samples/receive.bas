import websocket as ws

conn = ws.create("ws://127.0.0.1:8000/")
delay 20
while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print "Received:" + msg
  endif
  delay 10
wend
