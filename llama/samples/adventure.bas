import llm

' Configuration
const n_ctx = 5000
const n_batch = 512
const model_path = "models/Qwen_Qwen2.5-1.5B-Instruct-GGUF-Q4/qwen2.5-1.5b-instruct-q4_k_m.gguf"
const max_turns = 10

' Initialize two separate LLM instances
const storyteller = llm.llama(model_path, n_ctx, n_batch)
const player = llm.llama(model_path, n_ctx, n_batch)

' Configure Storyteller (creative, descriptive)
storyteller.set_max_tokens(150)
storyteller.set_temperature(0.8)
storyteller.set_top_k(80)
storyteller.set_top_p(0.95)
storyteller.set_min_p(0.05)
storyteller.set_penalty_repeat(1.2)
storyteller.set_penalty_last_n(128)

' Stop sequences to prevent storyteller from acting as player
storyteller.add_stop("\nPlayer:")
storyteller.add_stop("\nYou:")
storyteller.add_stop("\nI ")
storyteller.add_stop("\n\n\n")

' Configure Player (focused, action-oriented)
player.set_max_tokens(150)
player.set_temperature(0.9)
player.set_top_k(60)
player.set_top_p(0.9)
player.set_min_p(0.08)
player.set_penalty_repeat(1.15)
player.set_penalty_last_n(64)

' Stop sequences to keep player from narrating outcomes
player.add_stop("\nStoryteller:")
player.add_stop("\nNarrator:")
player.add_stop("\nSuddenly")
player.add_stop("\nThe ")
player.add_stop("\n\n")

' Game state
story_history = ""
turn_count = 0

' Initial storyteller prompt
storyteller_prompt = "You are a terse storyteller. IMPORTANT: IN ABSOLUTELY NO MORE THAN 3 sentances, "
storyteller_prompt += "begin an adventure story by describing the setting and initial situation for the hero. End with a situation that requires action.\n"
storyteller_prompt += "REMEMBER: keep it short !!!\n\nBegin the story:"

print "=== AI ADVENTURE GAME ==="
print "Storyteller and Player will create a story together"
print "========================="
print

' Main game loop
while turn_count < max_turns
  
  ' === STORYTELLER TURN ===
  print "\e[32mSTORYTELLER (Turn " + str(turn_count + 1) + "):"
  print "---"
  
  full_storyteller_prompt = storyteller_prompt + "\n\nPrevious story:\n" + story_history
  
  iter = storyteller.generate(full_storyteller_prompt)
  storyteller_response = ""
  while iter.has_next()
    chunk = iter.next()
    print chunk;
    storyteller_response = storyteller_response + chunk
  wend
  print
  print
  
  ' Update story history
  story_history = story_history + "\nStoryteller: " + storyteller_response
  
  ' Check if story should end
  if instr(lcase(storyteller_response), "the end") > 0 or instr(lcase(storyteller_response), "they lived") > 0 then
    print "=== THE STORY CONCLUDES ==="
    exit loop
  fi
  
  ' === PLAYER TURN ===
  print "\e[33mPLAYER (Turn " + str(turn_count + 1) + "):"
  print "---"
  
  player_prompt = "You are the hero in an adventure story. Based on the current situation, decide what action to take. Be decisive and specific. Respond in 1-2 sentences describing your action.\n\nCurrent situation:\n" + storyteller_response + "\n\nYour action:"
  
  iter = player.generate(player_prompt)
  player_response = ""
  while iter.has_next()
    chunk = iter.next()
    print chunk;
    player_response = player_response + chunk
  wend
  print
  print
  
  ' Update story history
  story_history = story_history + "\nPlayer: " + player_response
  
  ' Update storyteller prompt for next turn
  storyteller_prompt = "Continue the adventure story. The hero has taken an action. Describe what happens next as a result. Keep it to 2-3 sentences. Create tension, conflict, or new discoveries. End with a new situation requiring action.\n\nHero's last action: " + player_response + "\n\nWhat happens next:"
  
  turn_count = turn_count + 1
  
  ' Optional: pause between turns
  delay 500
wend

print
print "========================="
print "Game completed after " + str(turn_count) + " turns"
print "========================="
