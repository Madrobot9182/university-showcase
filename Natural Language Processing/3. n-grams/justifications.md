### Task 1 - Data Preparation

`data-create.py` contains the script I used to create the `train.txt` and `test.txt`. I simply randomy assigned 80% and 20% of the files to each output respectively. 

- Each line in the original `.cha` was copied line by line, so that all the sentences are preserved. This way I can just say the begin-of-utterance and end-of-utterance are the start and end of each line, simplifying the training and testing process

- I counted the potential OOV (commented lines using Counter), and I found that a token would never goes below 15~ occurances. I figured this was sufficiennt and did not do any additional processing for the training data.

### Task 2 - Training N-gram Models

- For each line in the training file, I first append `<s> | </s>` or `<s> <s> | </s> </s>` for bigrams and trigrams, before creating a Counter for the number of times a token(s) appears in the file
  
  - From [nlp - Computing N Grams using Python - Stack Overflow](https://stackoverflow.com/questions/13423919/computing-n-grams-using-python) I found this incredible method to create the ngram per line: `ngram.extend(zip(*[line[i:] for i in range(n)]))`, which is very compact and super efficient. 

- Then I normalize the Counts given the vocabulary size (assumed to be the size of the Unigram), and dump the result into the model.json
  
  - Laplace smoothing was also applied for all existing elements. The vocabulary size and a cached dictionary of "contexes" is also saved in the model.json for use during training
  - Context is just the running sum of the first 1/2 tokens in the bigram/trigram, used to speed up training and make testing easier. Ie `P(*|context)`

- The final output is a json of 7 elements, where 5 are the trained models with probabilities, the vocabulary size, and the context as a dictionary with counts.

### Task 3 - Evaluating N-gram Models

- On every line, we first append `<s> | </s>` or `<s> <s> | </s> </s>` if needed, recompute the ngram, and query the selected model type to get the value
  
  - If the value does not exist, I either assign it the probability of the entire vocabulary `vocab_size^ngram-dim`, or with laplace smoothing `1 / (context_size + vocab_size)`  as seen in class.
    
    

- I take the running log probability sum of each value, as python can't handle small enough probabilities without it. The final perplexity score is then:
  
  $$
  exp(-\frac{1}{token \ count} \ * \ log \ probabilities \ sum)
  $$
