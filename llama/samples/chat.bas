import llm

const n_ctx = 4000
const n_batch = 8000
const model_1 = "models/Qwen_Qwen2.5-3B-Instruct-GGUF-Q4/qwen2.5-3b-instruct-q4_k_m.gguf"
const llama = llm.llama(model_1, n_ctx, n_batch)

rem creative, storytelling
llama.set_max_tokens(5500)
llama.set_temperature(.9)
llama.set_top_k(50)
llama.set_top_p(0.1)
llama.set_min_p(0.01)
llama.set_penalty_repeat(1.1)
llama.set_penalty_last_n(64)

prompt = "You are an expert programmer in GPL SmallBASIC. Create a turn based game including fun ascii art\n"
prompt += "Here is an example of a type of class contruct in SmallBASIC which you may use\n"
prompt += "func Set\n"
prompt += "  sub add(byref item)\n"
prompt += "    local el \n"
prompt += "    el.key = item.parent_key\n"
prompt += "    el.map = item.map\n"
prompt += "    self.items[item.key] = el\n"
prompt += "  end\n"
prompt += "  func contains(byref item)\n"
prompt += "    return self.items[item.key].key != 0\n"
prompt += "  end\n"
prompt += "  func get(byref key)\n"
prompt += "    return self.items[key]\n"
prompt += "  end\n"
prompt += "  local result = {}\n"
prompt += "  result.items = {}\n"
prompt += "  result.add = @add\n"
prompt += "  result.contains= @contains\n"
prompt += "  result.get = @get\n"
prompt += "  return result\n"
prompt += "end\n"
prompt += "IMPORTANT: this dialect does not support the class keyword. Do not use it !!!\n"
prompt += "Always end a FUNC with the RETURN statement. For routines the do not return a value, use SUB\n"
prompt += "Use the LOCAL keyword inside a SUB or FUNC to declare local variables\n"
prompt += "Never declare FUNCTION, use FUNC instead\n"

print prompt;
iter = llama.generate(prompt)
while iter.has_next()
  print iter.next();
wend

print
print "==========="
print "tokens/sec "; iter.tokens_sec()
