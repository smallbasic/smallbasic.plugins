rem
rem generate api methods from api.json
rem

tload "api.json", s, 1
api = array(s)
ioioApi = [{
  "name": "beginBatch"
  "comment": "Start a batch of operations. This is strictly an optimization and will not change functionality"
},{
  "name": "disconnect",
  "comment": "Closes the connection to the board, or aborts a connection process started with waitForConnect()"
},{
  "name": "start",
  "comment": "Starts processing"
},{
  "name": "endBatch",
  "comment": "End a batch of operations."
},{
  "name": "hardReset",
  "comment": "Equivalent to disconnecting and reconnecting the board power supply."
},{
  "name": "softReset",
  "comment": "Resets the entire state (returning to initial state), without dropping the connection."
},{
  "name": "sync",
  "comment": "Sends a message to the IOIO and waits for an echo."
},{
  "name": "waitForConnect",
  "comment": "Establishes connection with the IOIO board."
},{
  "name": "waitForDisconnect",
  "comment": "Blocks until IOIO has been disconnected and all connection-related resources have been freed so that a new connection can be attempted."
}]

func get_signature(method)
  local result

  if (method.rtn == "void") then
    result = "void"
  else if (method.rtn == "boolean" || method.rtn == "int") then
    result = "int"
  else if (method.rtn == "float") then
    result = "float"
  endif

  result += " " + method.name + "("

  if (method.arg == "void") then
    result += "void"
  else if (method.arg == "boolean" || method.arg == "int") then
    result += "int"
  else if (method.arg == "float") then
    result += "float"
  endif

  result += ")"

  return result
end

print "# IOIO for SmallBASIC"
print
print "see: https://github.com/ytai/ioio/wiki"
print

print "## IOIO"
print
print "| Name    | Description   |"
print "|---------|---------------|"
for obj in ioioApi
  print "| void ioio." + obj.name + "()|" + obj.comment + "|"
next s
print

for obj in api
  print "## " + obj.name
  print
  print obj.comment
  print
  print "`io = ioio.open" + obj.name + "(pin)`"
  print ""
  print "| Name    | Description   |"
  print "|---------|---------------|"
  for method in obj.methods
    print "|" + get_signature(method) + "|" + method.comment  + "|"
  next
  print
next

