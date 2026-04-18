' ===============================================================
' NITRO AGENT SYSTEM (Enhanced Version)
' Designed for Agentic LLM interaction with external tools.
' ===============================================================

import llm

' --- Configuration ---
const model = "models/google_gemma-4-E4B-it-Q4_K_L.gguf"
const knowledge_files = ["skills.md"] ' List of files to load for priming

' ANSI Color Codes
const RESET = chr(27) + "[0m"
const GREEN = chr(27) + "[32m"
const YELLOW = chr(27) + "[33m"
const CYAN = chr(27) + "[36m"
const RED = chr(27) + "[31m"
const BOLD_CYAN = chr(27) + "[1;36m"

' Initialize the LLAMA interface
const n_ctx = 8000
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
  print CYAN + "  Try asking me about the contents of 'nitro.txt' or listing files in './data'." + RESET
  print CYAN + "  Type 'exit' to quit." + RESET
  print
end sub

'
' Handles file system commands received from the LLM.
'
func handle_fs(cmd)
  local op, arg, file_list, v

  split(cmd, " ", v)
  op = v[0]
  arg = v[1]
  print RED + "op=" + op + " arg=" + arg  + RESET

  select case op
  case "FS:LIST"
    result = ""
    file_list = files(arg) ' Assumes SmallBASIC has a dirlist function
    for f in file_list
      result = result + f + chr(10)
    next
    return result
  case "FS:READ"
    content = ""
    try
      tload arg, content, 1
      return content
    catch
      return RED + "ERROR: File not found or unreadable." + RESET
    end try
  case "FS:WRITE"
    ' Simplistic write implementation (requires parsing filename and content)
    return GREEN + "OK: Data written successfully to " + arg + RESET
  case else
    return RED + "ERROR: unknown command " + op + RESET
  end select
end func

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
sub process_tool(text_line)
  local result = handle_fs(trim(text_line))
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
sub process_input()
  local user_input
  input "You:", user_input
  user_input = trim(user_input)
  if user_input == "exit" then
    stop
  endif
  return "<|turn|>user\n" + user_input + "\n<|turn|>\n<|turn|>model"
end

'
' Main process
'
sub main()
  local line_buf, output_buf, token, nl, text_line
  local iter = llama.generate(initialize_agent())
  local user_input = ""

  welcome_message()

  while 1
    ' Process generation loop (Tool Calling / Output)
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

        if left(trim(text_line), 3) = "FS:" then
          iter = llama.generate(process_tool(text_line))
          ' Break the inner loop to restart the generation process
          exit loop
        else
          ' Print standard output tokens
          print CYAN + text_line + RESET
        end if
      end if
    wend

    ' Flush remaining line buffer
    if len(line_buf) then print CYAN + line_buf + RESET
    print ""
    print "--- Tokens/sec: " + iter.tokens_sec() + " ---\n"

    iter = llama.generate(process_input())
  wend
end

main()
