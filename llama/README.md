#  Generator settings

## factual answers, tools, summaries

```
llama.set_max_tokens(150)
llama.set_temperature(0.0)
llama.set_top_k(1)
llama.set_top_p(0.0)
llama.set_min_p(0.0)
```

## assistant, Q+A, explanations, chat

```
llama.set_max_tokens(150)
llama.set_temperature(0.8)
llama.set_top_k(40)
llama.set_top_p(0.0)
llama.set_min_p(0.05)
```

## creative, storytelling

```
llama.set_max_tokens(20)
llama.set_temperature(1.0)
llama.set_top_k(80)
llama.set_top_p(0.0)
llama.set_min_p(0.1)
```

## surprises

```
llama.set_max_tokens(200)
llama.set_temperature(1.2)
llama.set_top_k(120)
llama.set_top_p(0.0)
llama.set_min_p(0.15)
```

## technical, conservative

```
llama.set_max_tokens(150)
llama.set_temperature(0.6)
llama.set_top_k(30)
llama.set_top_p(0.0)
llama.set_min_p(0.02)
```

## speed optimised on CPU

```
' llama.set_max_tokens(10)
' llama.set_temperature(0.7)
' llama.set_top_k(20)
' llama.set_top_p(0.0)
' llama.set_min_p(0.05)
```

# Avoiding repetition

## Conservative - minimal repetition control

```
llama.set_penalty_last_n(64)
llama.set_penalty_repeat(1.05)
```

## Balanced - good default

```
set_penalty_last_n(64)
set_penalty_repeat(1.1)
```

## Aggressive - strong anti-repetition

```
set_penalty_last_n(128)
set_penalty_repeat(1.2)
```

## Disabled

```
llama.set_penalty_last_n(0)
llama.set_penalty_repeat(1.0)
```
