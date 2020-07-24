import websocket

rem receive
ws = websocket.listen(8080)
while ws.open()
  # read the next message from one of the connections
  msg = ws.receive()
  ws.post("cats")
  
wend

