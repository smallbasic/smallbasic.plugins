' ===============================================================
' NITRO AGENT SYSTEM (Enhanced Version)
' Designed for Agentic LLM interaction with external tools.
' ===============================================================

import llm

' --- Configuration ---
const model = "models/google_gemma-4-E4B-it-Q4_K_L.gguf"
const knowledge_files = ["nitro.md"]

' ANSI Color Codes
const RESET = chr(27) + "[0m"
const GREEN = chr(27) + "[32m"
const YELLOW = chr(27) + "[33m"
const BLUE = chr(27) + "[34m"
const CYAN = chr(27) + "[36m"
const RED = chr(27) + "[31m"
const BOLD_CYAN = chr(27) + "[1;36m"
const CHANNEL_END = "<channel|>"

' llama configuration
const n_ctx = 8000
const n_batch = 512
const n_max_tokens = 4096
const n_temperature = 0.2
const n_top_k = 40
const n_top_p = 0.9
const n_min_p = 0.05
const n_penalty_repeat = 1.1
const n_penalty_last_n = 256

sandbox_home = cwd

'
' Displays the welcome message
'
sub welcome_message()
  print
  print BOLD_CYAN;
  print "          .  ·    ✦        .    ·      "
  print "     ·         .        ·              "
  print "        ✦   P · I · C · A · R · D   ✦  "
  print "              .    ·         .         "
  print "     .    ·        ✦    .        ·     "
  print
  print BOLD_CYAN + "  P I C A R D   A G E N T   S Y S T E M   v1.0" + RESET
  print
  print CYAN + "  >> Welcome to Picard! Your AI Agent Companion. << " + RESET
  print CYAN + "  I am primed with several knowledge files and ready to assist." + RESET
  print CYAN + "  Try asking me about the contents of 'nitro.md' or listing files in './data'." + RESET
  print CYAN + "  Type 'exit' to quit." + RESET
  print
end sub

'
' handles the TOOL:LIST command
'
func list_files(arg)
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
func read_file(arg)
  try
    tload sandbox_home + arg, result, 1
  catch
    result = "ERROR: File not found or unreadable."
  end try
  return result
end  

'
' handles the TOOL:WRITE command
'
func write_file(arg)
  result = "OK: Data written successfully to " + arg
  return result
end

'
' Handles file system commands received from the LLM.
'
func handle_cmd(cmd)
  local v, result

  split(cmd, " ", v)
  local op = v[0]
  local arg = iff(len(v) == 2, v[1], "")
  
  print RED + "TOOL:" + op + " - " + arg + RESET

  select case op
  case "TOOL:DATE"
    result = date
  case "TOOL:TIME"
    result = time
  case "TOOL:RND"
    result = rnd
  case "TOOL:LIST"
    result = list_files(arg)
  case "TOOL:READ"
    result = read_file(arg)
  case "TOOL:WRITE"
    result = write_file(arg)
  case else
    result = "ERROR: unknown command " + op
  end select

  print RED + "TOOL RESULT:" + result + RESET
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

  ' Set the initial system prompt for the LLM
  print YELLOW;
  print "  ╔═══════════════════════════════════════╗"
  print "  ║  > PICARD_                            ║"
  print "  ║  > STATUS: ENGAGED                    ║"
  print "  ║  > STARDATE: 42026.421                ║"
  print "  ║  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ 100% READY      ║"
  print "  ╚═══════════════════════════════════════╝"
  print
  print RESET
  return prompt
end

'
' Execute the given tool
'
func process_tool(tool)
  return handle_cmd(trim(tool))
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

  while 1
    local buffer = ""
    local text_colour = BLUE

    while iter.has_next()
      buffer += iter.next()
      local chan_end = instr(buffer, CHANNEL_END)

      if chan_end != 0 then
        ' print buffer up to channel_end
        buffer = left(buffer, chan_end - 1)
        print text_colour + buffer + RESET
        print

        ' print buffer following channel_end
        text_colour = CYAN        
        print text_colour + mid(buffer, chan_end + len(CHANNEL_END)) + RESET;
        
        ' reset buffer
        buffer = ""
      endif

      ' Only print non-command tokens
      local nl = instr(buffer, chr(10))
      if nl then
        local text_line = left(buffer, nl - 1)
        buffer = mid(buffer, nl + 1)
        if text_line == "</|think|>" then
          text_colour = CYAN
        else
          print text_colour + text_line + RESET
        end if
      end if
    wend

    ' Flush remaining line buffer
    if len(buffer) > 0 and left(trim(buffer), 5) == "TOOL:" then
      ' TOOL:xxx should always appear on the final line
      iter = llama.add_message("tool", process_tool(buffer))
    else
      if len(buffer) > 0 then
        ' TODO: trim any trailing <|turn|>
        print text_colour + buffer + RESET
      endif
      print
      print "--- Tokens/sec: " + round(iter.tokens_sec(), 2) + " ---\n"
      iter = llama.add_message("user", process_input())
    endif
  wend
end

welcome_message()
main()
'print list_files(".")
