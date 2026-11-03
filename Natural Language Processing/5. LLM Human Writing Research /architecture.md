## Did you design your solution with classes?

```
No I did not.
The training and evaluation logic wasn't complicated enough to need encapsulating everything into classes. 
```

## Does the use of classes mean you have to type more code?

```
You'd absolutely need more code if you replace functions with classes. Constructors, parameters, inner functions, operator overrides etc.
If I did move all functions into a class, my main would just call the functions in the class same as before. There would just be another layer of abstraction to deal with.
```

---

## Functions used for model training

#### Functions you wrote with their equivalent in the LLM code (or N/A if applicable)

```
## train.py
main():
    - LLM structured very similarly, where the file is read, context is found, n-grams trained, and finally saved to json file.
    - LLM reads file in read_data(filepath), and finds vocab in get_vocabulary(utterances)
find_ngrams(input_list, n):
    - LLM separated training into train_unigram(utterances), train_bigram(utterances), train_trigram(utterances). Start and end tokens inserted in add_boundary_markers(utterances)
```



#### Functions the LLM wrote that have no equivalent in your code

```
## train.py
counter_to_normalized_json(ngram_count, n, context_counts, lap_smoothing=False, vocab_size=0):
    - LLM stores raw counts in model instead of normalizing to probabilities in train.
```

#### Discussion of the similarities and differences between the two designs.

```
What's similar is the basic flow of training, where both me and the AI read file -> get ngrams/context/vocab -> output model
However, the LLM (Claude) loves to break everything down into functions and document it meticulously. It also split ngram operations into separate unigram/bigram/trigram functions.
LLM's model also just stored the ngram counts, then calcualted the probabilities in eval while finding perplexity. End result is the same, just moving work from train into eval.
I wonder what kind of data that Claude was trained on, and how that data results in excessive verbose functions, detailed comments, and tons of debug printing.
```

#### Which design is better?

```
On the tradeoff between how much code should be encapuslated into functions, I think the LLM abstracted too much for what should be a pretty short ~150 line file. 
Functions (with good names) helps with a first glance in main(), but readability and ease of debugging suffers when I have to jump between many lines and keep track of what/where variables are being passed around.
Functions are useful when code needs to be repeated multiple times.
My find_ngrams(input_list, n), can take in arbitary dimensions and can be reused to find uni/bi/trigrams. The LLM wrote 3 different functions for each, which repeats lines unecessary and adds more work to bookeep and debug.
The LLM models stores counts, while my model stores the normalized data, with and without laplace smoothing. While this does create a bigger file (I have 5 modesls instead of 3), it should also speed up evaluation as I don't need to calcuate all of this on the fly. Training can be slow since it only runs once, but evaluating with multiple datasets and models means more time spent querying models. Hence, my method is better in terms of efficiency.
While premature optimizing for shorter files is a bad practice, shortening the code (where reasonable) makes for cleaner and easier to use programs. The LLM code is correct, but could definitely be shortened, making it easier to digest.
```

---

## Functions used for calculating perplexity

#### Functions you wrote with their equivalent in the LLM code (or N/A if applicable)

```
## eval.py (test.py in my version)
main(): (contains all my logic)
    - LLM also structured similar to my main, but with logic moved to other functions
    - LLM reads file in read_data(filepath), and inserts start end tokens in add_boundary_markers(utterances)
    - LLM separated ngram perplexities into separate compute_unigram_perplexity(model_data, utterances), compute_bigram_perplexity(model_data, utterances), compute_trigram_perplexity(model_data, utterances)#### Functions the LLM wrote that have no equivalent in your code
```

```
N/A
```

#### Discussion of the similarities and differences between the two designs.

```
Same notes with training, the LLM follows the same flow as me, but breaks everything down into many function, with many comments and debug statements
My train.py (eval.py for LLM), is just one main() with no functions or extra abstraction, while the LLM generated 5 helper functions and over double my line count.
I believe since the default prompts of all commercial AI goes somewhere along the lines of "be an helpful assistance", it interprets that as "guiding the user" along a train of thought with functions being the steps. Of course LLM don't really "think", even reasoning models just generates steps, the reprompt themselves to hopefully improve their response.
```

#### Which design is better?

```
Like with train.py, I think the LLM abstracted too much for what should be a much shorter file. 
It's much easier to trace my train.py (eval.py for LLM) when it's all in main(), and you can trace every step one line at a time. Eval only needs to evaluate one kind of model, with or without laplace smoothing, and doesn't need multiple functions to simplify calcualting multiple models and parameters. Hence my method makes it easy to read and debug.
While premature optimizing for shorter files is a bad practice, shortening the code (where reasonable) makes for cleaner and easier to use programs. The LLM code is correct, but could definitely be shortened, making it easier to digest.
```

---

## Functions used for printing results

#### Functions you wrote with their equivalent in the LLM code (or N/A if applicable)

```
## eval.py
main(): calculates and prints the perplexity at the end
    - LLM also prints perplexity in its main()
```

#### Functions the LLM wrote that have no equivalent in your code

```
## utils.py (only in LLM)
I do not have an utils.py, or any other method to output more than just the perplexity in eval.py
The entirety of utils.py is just to print model statistics. eval.py already finds and prints the perplexity, every other function is just extra.
```

#### Discussion of the similarities and differences between the two designs.

```
My main() in train.py (eval.py for LLM) is actually more verbose, printing perplexity and number of unseen tokens. LLM just prints perplexity and rounds to two decimals.
The biggest difference is definitely the extra utils.py with a lot of extra stats I didn't track. LLM code tends to print much more information than humans. It's also usually formatted nicely even though most programmers I know can't be bothered with text allignment and formatting in the console
```

#### Which design is better?

```
Depending on how much time you have to program everything, there's no real downsides to having more infomation about the model performance and statsistics. Although, unless you need all that infomation for the task at hand, then it's not really necessary to code  all that logic for nothing.
For sake of completeness, LLM's is better than my code for printing results.
```
