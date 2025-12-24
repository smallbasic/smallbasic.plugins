import llm

const model = "models/Qwen_Qwen2.5-1.5B-Instruct-GGUF-Q4/qwen2.5-1.5b-instruct-q4_k_m.gguf"
const llama = llm.llama(model, 4096, 512)

llama.set_max_tokens(150)
llama.set_min_p(0.5)
llama.set_temperature(.8)
llama.set_top_k(1)
llama.set_top_p(0)


rem factual answers, tools, summaries
' llama.set_max_tokens(150)
' llama.set_temperature(0.0)
' llama.set_top_k(1)
' llama.set_top_p(0.0)
' llama.set_min_p(0.0)

rem assistant, Q+A, explanations, chat
' llama.set_max_tokens(150)
' llama.set_temperature(0.8)
' llama.set_top_k(40)
' llama.set_top_p(0.0)
' llama.set_min_p(0.05)

rem creative, storytelling
' llama.set_max_tokens(200)
' llama.set_temperature(1.0)
' llama.set_top_k(80)
' llama.set_top_p(0.0)
' llama.set_min_p(0.1)

rem surprises/loko
' llama.set_max_tokens(200)
' llama.set_temperature(1.2)
' llama.set_top_k(120)
' llama.set_top_p(0.0)
' llama.set_min_p(0.15)

rem technical, conservative
' llama.set_max_tokens(150)
' llama.set_temperature(0.6)
' llama.set_top_k(30)
' llama.set_top_p(0.0)
' llama.set_min_p(0.02)

rem speed optimised on CPU
llama.set_max_tokens(150)
llama.set_temperature(0.7)
llama.set_top_k(20)
llama.set_top_p(0.0)
llama.set_min_p(0.05)

' // Conservative - minimal repetition control
' _penalty_last_n = 64;
' _penalty_repeat = 1.05f;

' // Balanced - good default
' _penalty_last_n = 64;
' _penalty_repeat = 1.1f;

' // Aggressive - strong anti-repetition
' _penalty_last_n = 128;
' _penalty_repeat = 1.2f;

' // Disabled
' _penalty_last_n = 0;
' _penalty_repeat = 1.0f;

llama.set_penalty_repeat(1.15)
llama.set_penalty_last_n(64)



llm_prompt = """\
you are a helpful assistant\
 \nQuestion: when is dinner?\
"""

print llm_prompt
print llama.generate(llm_prompt)

' iter = llama.generate(llm_prompt)
' while iter != 0
'   print iter.next()
' wend
