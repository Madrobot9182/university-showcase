# Answer these questions with respect to the data in the `training` set.

## Does the LLM handle begin and end of sentence tokens across models correctly?

```
No it does not. add_boundary_marker(utterances) only adds one start and end token to each line, which means the trigram will miss the first and last utterance (eg <s> <s> TOK). The unigram and bigram training handles this correctly, but train_trigram will be incorrect 
```

### Does your code handle these tokens in the same way?

```
My find_ngram function takes each line in the data file, appends [0-2] start and end tokens, then uses python zip() to create [1-3] length tuples that represent my ngrams. This is a much more flexible approach that could be extended to any degree ngram and reused multiple times for to train any dimensional model.
```

----

## Does the LLM count n-grams correctly?

```
The LLM code does count ngrams correctly. For each train function, it keeps a running sum for each word and corresponding context, and also stores the context count for bigram and trigrams.
```

### Does your code count n-grams in the same way?

```
In my implementation, my find_ngrams() keeps a list of zips containing my ngrams, from which I convert to a python Counter(). It automatically counts all my ngrams without needing to iterate through all the utterances manually.
```

----

## Is the model produced by the LLM correct?

```
Accordingly to the assignment: "[the model] should store actual N-gram counts". The LLM code does store a dictionary of utterances to integer counts, which does match the requirement. Although, since the trigram start and end tags are incorrectly created, the trigram counts are techically incorrect as well.
```

### Is the model produced by your code different?

```
I instead find the normalized laplace and unsmoothed probabilities for the ngrams, and store the 5 probability models in my json. This does contradict the project requirement, even though my results are still the same. In the end, it is better to just store the raw counts, as that information is preserved and couldreused for other tasks that needs the raw counts.
```

----

# Other LLM code that is **incorrect**

```
N/A
```