
const llama = llm.llama("qwen.gguf", 1024)

print llama.generate("Write a BASIC program", 256, 0.2)

print llama.chat("Hello")
print llama.chat("Write a BASIC program to draw a cat")
print llama.chat("Now add color")

llama.reset()

print llama.chat("Who are you?")
