# Answer these questions with respect to the data in the `dev` set.

## Does the LLM handle begin and end of sentence tokens across models correctly?

```
No it does not. Just like in train.py, add_boundary_marker(utterances) only adds one start and end token to each line, which means the trigram will miss the first and last utterance (eg <s> <s> TOK). The unigram and bigram training handles this correctly, but train_trigram will be incorrect.
```

### Does your code handle these tokens in the same way?

```
When appending the start and end tokens, I actually check the dimension of what model is being used, and append the correct amount of tokens to each sentence. This is correct behaviour and produces the correct perplexity from my correct model.
```

----

## Does the LLM count n-grams correctly?

```
Yes the counting logic is still correct for the dev set. The dev file is also space seperated lines of utterances, and is still correctly processed the same like the tiny set. 
```

### Does your code count n-grams in the same way?

```
In my implementation, my find_ngrams() keeps a list of zips containing my ngrams, from which I convert to a python Counter(). It automatically counts all my ngrams without needing to iterate through all the utterances manually.
```

----

## Does the LLM handle OOV tokens correctly?

```
For OOV words, the LLM uses dict.get() and returns a default value of zero since it's not in the ngram. Laplace smoothing can handle this so no extra checks is needed. For unsmoothed models a default value of 1e-10 is set to prevent taking the log of zero.
Mathmatically, a umsmoothed OOV token should have 0 probability, resulting in infinite perplexity. This method is really a hack that alters the distribution. At the very least, it's not a technique mentioned in the textbook. Neither smoothing nor discounting.
```

### Does your code handle OOV tokens in the same way?

```
The only difference between the LLM implementaiton and mine is that my default probability for OOV token is 1 / (vocab-size)^n-dims instead of 1e-10. I figured this would at least add some connection betweent the default value and the model.
In retrospect, I should have processed the text for low occurance utterances, replace those with <UNK> tokens, and use the <UNK> count as my default probability instead. This would be much more correct than both mine and the LLMs implementation.
```

----

## Does the LLM compute perplexity correctly?

```
The calculation for perplexity is correct, as each compute_[unigram|bigram|trigram]_function() correctly finds the running log base 2 sum from the dev set, and finds the perplexity as 2^(-log sum / length of input). This is the same as the multiples of raw probabilities seen in the textbook, but using logs prevents unerflows.
```

### Does your code compute perplexity in the same way?

```
My formula ends up being the same, except I took the log base e, and raised the expression to the power of e instead of 2. Between e and 2, varying sources seem to use both, and so I can't determine a definitivly correct choice. Nevertheless, both will produce the same value so it doesn't really matter.
```

----

## Does the LLM perform add-one smoothing corectly?

```
The add one smoothing (laplace) is correct. For both bigram and trigram laplace, they use the contexes stored from training to find the counts for both ngram and context. Then it's just ngram_count + 1 / context_count + vocab_size, which is what we want.
```

### Does your code perform add-one smoothing in the same way?

```
Since I've precalcuated all the laplace values in separate models, I just fetch the values and use that in my calculations. It's only when the ngram is not found that I attempt some backoff, then default to the value 1 / vocab_size. While this does speed up evaluation times, I do think I should have stored the raw counts and calculated the probability in eval.py instead like the LLM, as information is lost, and I need to train a model for every kind of smoothing in my case. 
```

----

# Other LLM code that is **incorrect**

```
N/A
```