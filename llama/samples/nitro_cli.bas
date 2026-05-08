' ===============================================================
' NITRO AGENT SYSTEM (Enhanced Version)
' Designed for Agentic LLM interaction with external tools.
' ===============================================================

import llm

' --- Configuration ---
const model = "models/Qwen3.5-9B-Q4_K_M.gguf"
const knowledge_files = ["nitro.md"]
const code_files = [".py", ".cpp", ".h", ".bas", ".java", ".html", ".js", "jsp", ".tag"]

' ANSI Color Codes
const RESET = chr(27) + "[0m"
const GREEN = chr(27) + "[32m"
const BLUE = chr(27) + "[34m"
const CYAN = chr(27) + "[36m"
const RED = chr(27) + "[31m"
const WHITE = chr(27) + "[37m"
const BOLD_CYAN = chr(27) + "[1;36m"

' llama configuration (quen settings)
const n_ctx = 32768
const n_batch = 512
const n_max_tokens = 4096
const n_temperature = 0.6
const n_top_k = 20
const n_top_p = 0.95
const n_min_p = 0
const n_penalty_repeat = 1.0
const n_penalty_last_n = 256

sandbox_home = cwd

'
' Displays the welcome message
'
sub welcome_message()
  print
  print BOLD_CYAN + "  N I T R O   A G E N T   S Y S T E M   v1.0" + RESET
  print
  print CYAN + "  >> Welcome to Nitro! Your AI Agent Companion. << " + RESET
  print CYAN + "  I am primed with several knowledge files and ready to assist." + RESET
  print CYAN + "  Try asking me about the contents of 'nitro.md' or listing files in './data'." + RESET
  print CYAN + "  Type 'exit' to quit." + RESET
  print
end sub

'
' handles the TOOL:LIST command
'
func tool_list_files(arg)
  if (arg == "./") then
    arg = sandbox_home + arg
  else if (len(arg) == 0 or arg == ".") then
    arg = sandbox_home
  endif

  local result = []

  func walker(node)
    if (node.depth == 0) then
      if (node.dir && left(node.name, 1) != ".") then
        result << "[" + node.name + "]"
      else
        result << node.name
      endif
    endif
    return node.depth == 0
  end

  dirwalk arg, "", use walker(x)
  return str(result)
end

'
' handles the TOOL:READ command
'
func tool_read_file(arg)
  try
    tload sandbox_home + arg, result, 1
  catch
    result = "ERROR: File not found or unreadable."
  end try
  return result
end

'
' removes markdown backticks from code blocks
'
func strip_code_fences(filename, s)
  local result = s
  local dot = instr(filename, ".")
  local extn = mid(filename, dot)

  if (extn in code_files == 0) then
    return result
  endif

  local pos = instr(s, "```")
  if (pos) then
    local nl = instr(pos + 3, s, chr(10))
    if (nl) then
      result = mid(s, nl + 1)
      pos = instr(result, "```")
      if (pos) then
        result = left(result, pos - 1)
      endif
    endif
  endif
  return result
end

'
' handles the TOOL:WRITE command
'
func tool_write_file(arg, s)
  try
    tsave sandbox_home + arg, s
    result = "OK: Data written successfully to " + arg
  catch e
    result = "ERROR: " + e
  end try
  return result
end

'
' handles the TOOL:PERMISSION command
'
func tool_permission()
  local k
  input "Agree?"; k
  return iff(trim(k) == "YES", "YES", "NO")
end

'
' Handles file system commands received from the LLM.
'
func process_tool(cmd)
  local result, op, arg1, arg2

  local pos1 = instr(cmd, " ")
  if (pos1 > 0) then
    op = left(cmd, pos1 - 1)
    local pos2 = instr(pos1 + 1, cmd, " ")
    if (pos2 > 0) then
      arg1 = mid(cmd, pos1 + 1, pos2 - pos1 - 1)
      arg2 = mid(cmd, pos2 + 1)
    else
      arg1 = mid(cmd, pos1 + 1)
    endif
  endif

  ' print RED
  ' print "["+op+"]"
  ' print "["+arg1+"]"
  ' print "["+arg2+"]"
  ' print RESET

  select case op
  case "TOOL:DATE"
    result = date
  case "TOOL:TIME"
    result = time
  case "TOOL:RND"
    result = rnd
  case "TOOL:LIST"
    result = tool_list_files(arg1)
  case "TOOL:READ"
    result = tool_read_file(arg1)
  case "TOOL:WRITE"
    result = tool_write_file(arg1, strip_code_fences(arg1, arg2))
  case "TOOL:EXISTS"
    result = iff(exist(arg1), "YES", "NO")
  case "TOOL:PERMISSION"
    result = tool_permission()
  case else
    result = "ERROR: unknown command " + op
  end select

  'print RED + "TOOL RESULT:" + result + RESET
  return result
end

'
' Loads knowledge_files
'
func initialize_agent()
  local prompt = ""

  for file in knowledge_files
    content = ""
    try
      tload file, content, 1
      prompt = prompt + chr(10) + content + chr(10)
      print GREEN + "  ✅ Loaded knowledge file: " + file + RESET
    catch
      print RED + "  ❌ ERROR: Could not load " + file + ". Check path." + RESET
    end try
  next

  return prompt
end

'
' Returns the user user input
'
func process_input()
  local user_input
  input "You:? ", user_input
  user_input = trim(user_input)
  if user_input == "exit" OR user_input = "quit" then
    stop
  endif
  return user_input
end

'
' creates the llama instance
'
func create_llama()
  local llama = llm.llama(model, n_ctx, n_batch, 50)
  llama.add_stop("<|turn|>")
  llama.set_max_tokens(n_max_tokens)
  llama.set_temperature(n_temperature)
  llama.set_top_k(n_top_k)
  llama.set_top_p(n_top_p)
  llama.set_min_p(n_min_p)
  llama.set_penalty_repeat(n_penalty_repeat)
  llama.set_penalty_last_n(n_penalty_last_n)
  return llama
end

'
' Main process
'
sub main()
  ' note: this construct requires recent sbasic fixes
  local llama = create_llama()
  local iter = llama.add_message("system", initialize_agent())

  sub handle_think(s)
    if s == "<|think|>" then
      print BLUE;
    else if s == "</|think|>" then
      print WHITE;
    end if
  end

  while 1
    local buffer = ""

    while iter.has_next()
      buffer += iter.next()
      local nl = instr(buffer, chr(10))
      if nl then
        local text_line = left(buffer, nl - 1)
        buffer = mid(buffer, nl + 1)
        if left(trim(text_line), 5) == "TOOL:" then
          text_line += buffer + " " + iter.all()
          iter = llama.add_message("tool", process_tool(text_line))
          buffer = ""
        else
          print text_line
        endif
        handle_think(text_line)
      end if
    wend

    ' Flush remaining line buffer
    if len(buffer) > 0 and left(trim(buffer), 5) == "TOOL:" then
      iter = llama.add_message("tool", process_tool(buffer))
    else
      if len(buffer) > 0 then
        'print text_colour + buffer + RESET
        print buffer
        handle_think(buffer)
      endif
      print
      print WHITE;
      print "--- Tokens/sec: " + round(iter.tokens_sec(), 2) + " ---\n"
      iter = llama.add_message("user", process_input())
      print BLUE;
    endif
  wend
end

welcome_message()
main()
