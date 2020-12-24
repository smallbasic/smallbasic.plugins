import websocket as ws

conn = ws.create("ws://127.0.0.1:8000/")
if (ws.open(conn) == 1) then
  ws.send(conn, "Hello WebSocket server")
else
  print "not connected"  
endif

while ws.open(conn) == 1
  msg = ws.receive(conn)
  if (len(msg) > 0) then
    print "Received:" + msg
    input k
    if (len(k) != 0) then 
       ws.send(conn, k)
    endif
  endif  
  delay 10
wend



