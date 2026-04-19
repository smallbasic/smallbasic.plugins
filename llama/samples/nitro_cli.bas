' ===============================================================
' NITRO AGENT SYSTEM (Enhanced Version)
' Designed for Agentic LLM interaction with external tools.
' ===============================================================

import llm

' --- Configuration ---
const model = "models/google_gemma-4-E4B-it-Q4_K_L.gguf"
const knowledge_files = ["skills.md"]

' ANSI Color Codes
const RESET = chr(27) + "[0m"
const GREEN = chr(27) + "[32m"
const YELLOW = chr(27) + "[33m"
const BLUE = chr(27) + "[34m"
const CYAN = chr(27) + "[36m"
const RED = chr(27) + "[31m"
const BOLD_CYAN = chr(27) + "[1;36m"
const CHANNEL_MARKER = "<channel|>"

' Initialize the LLAMA interface
const n_ctx = 16000
const n_batch = 512
const llama = llm.llama(model, n_ctx, n_batch, 50)

llama.add_stop("<|turn|>")
llama.set_max_tokens(4096)
llama.set_temperature(0.2)
llama.set_top_k(40)
llama.set_top_p(0.9)
llama.set_min_p(0.05)           ' filter weak tokens
llama.set_penalty_repeat(1.1)   ' avoid loops
llama.set_penalty_last_n(256);  ' longer memory

'
' Displays the welcome message
'
sub welcome_message()
  print
  print BOLD_CYAN;
  print "      •   •    •   •••••   ••••     •••  "
  print "      ••  •    •     •    •   •    •   • "
  print "      •  ••    •     •    ••••    •     •"
  print "      •   •    •     •    •   •    •   • "
  print "      •   •    •     •    •   •     •••  "
  print
  print BOLD_CYAN + "  N I T R O   A G E N T   S Y S T E M   v1.0" + RESET
  print ""
  print CYAN + "  >> Welcome to Nitro! Your AI Agent Companion. << " + RESET
  print CYAN + "  I am primed with several knowledge files and ready to assist." + RESET
  print CYAN + "  Try asking me about the contents of 'skills.md' or listing files in './data'." + RESET
  print CYAN + "  Type 'exit' to quit." + RESET
  print
end sub

'
' Handles file system commands received from the LLM.
'
func handle_cmd(cmd)
  local op, arg, file_list, v, result

  split(cmd, " ", v)
  op = v[0]
  arg = iff(len(v) == 2, v[1], "")
  'print RED + "op=" + op + " arg=" + arg  + RESET

  select case op
  case "TOOL:DATE"
    result = date
  case "TOOL:TIME"
    result = time
  case "TOOL:RND"
    result = rnd
  case "TOOL:LIST"
    file_list = files(arg)
    for f in file_list
      result = result + f + chr(10)
    next
  case "TOOL:READ"
    try
      tload arg, result, 1
    catch
      result = "ERROR: File not found or unreadable."
    end try
  case "TOOL:WRITE"
    ' Simplistic write implementation (requires parsing filename and content)
    result = "OK: Data written successfully to " + arg
  case else
    result = "ERROR: unknown command " + op
  end select
  return result
end

'
' Loads knowledge_files then returns the following format:
'
' <|turn|>system
' {skills.md...}
' <|turn|>
'
func initialize_agent()
  local prompt = ""

  for file in knowledge_files
    content = ""
    try
      tload file, content, 1
      prompt = prompt + chr(10) + content + chr(10)
      print GREEN + "✅ Loaded knowledge file: " + file + RESET
    catch
      print RED + "❌ ERROR: Could not load " + file + ". Check path." + RESET
    end try
  next

  ' Set the initial system prompt for the LLM
  print YELLOW + "\n[ Nitro Agent Initialized Successfully! ]" + RESET
  print
  return "<|turn|>system\n" + prompt + "\n<|turn|>"
end

'
' Execute the given tool, then returns the following format:
'
' <|turn|>tool
' {tool_output}
' <|turn|>
' <|turn|>model
'
func process_tool(text_line)
  local result = handle_cmd(trim(text_line))
  return "<|turn|>tool\n" + result + "\n<|turn|>\n<|turn|>model"
end

'
' Process user input, then returns the following format
'
' <|turn|>user
' {user_input}
' <|turn|>
' <|turn|>model
'
func process_input()
  local user_input
  input "You:? ", user_input
  user_input = trim(user_input)
  if user_input == "exit" OR user_input = "quit" then
    stop
  endif
  return "<|turn|>user\n" + user_input + "\n<|turn|>\n<|turn|>model"
end

'
' Main process
'
sub main()
  local line_buf, output_buf, nl, text_line
  local iter = llama.generate(initialize_agent())
  local text_colour = BLUE

  welcome_message()

  while 1
    line_buf = ""
    output_buf = ""

    while iter.has_next()
      token = iter.next()
      line_buf = line_buf + token

      ' Only print non-command tokens
      nl = instr(line_buf, chr(10))
      if nl then
        text_line = left(line_buf, nl - 1)
        line_buf = mid(line_buf, nl + 1)
        if text_line == "</|think|>" then
          text_colour = CYAN
        else
          print text_colour + text_line + RESET
        end if
      end if
    wend

    ' Flush remaining line buffer
    if left(trim(line_buf), 5) == "TOOL:" then
      ' TOOL:xxx should always appear on the final line
      text_colour = BLUE
      iter = llama.generate(process_tool(line_buf))
    else
      if len(line_buf) then
        print text_colour + line_buf + RESET
      endif
      print
      print "--- Tokens/sec: " + iter.tokens_sec() + " ---\n"
      iter = llama.generate(process_input())
    endif
  wend
end

main()
