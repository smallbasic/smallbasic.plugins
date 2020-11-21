import raylib as rl

print rl.textformat("%s", "hello")
print rl.textformat("float [%.3f] int [%d] string [%s]", 123.999, 22, "cats")
print rl.textformat("%s%d%%%%", "hello")
print rl.textformat("result = %d%% good? [%s]", 100, "yes")
print rl.textformat("% ", "crash")
