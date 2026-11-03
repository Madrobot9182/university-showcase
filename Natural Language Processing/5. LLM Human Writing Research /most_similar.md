## Most similar functionality

### Your code

```Python
test.py: main()
```

### LLM code

```Python
eval.py: computer_[unigram|bigram|trigram]_perplexity()
```

### Output of `diff`

```
diffstat:
unknown |  539 +++++++++++++++++++++++++++++++++++++++++++++-------------------
1 file changed, 381 insertions(+), 158 deletions(-)
diff:
1,3c1,12
< import argparse
< from pathlib import Path
< from collections import Counter
---
> #!/usr/bin/env python3
> """
> Train n-gram language models (unigram, bigram, trigram) from phonetic sequence data.
> 
> Usage:
>     python3 src/train.py <input_file> <output_file>
> 
> Arguments:
>     input_file: Path to the training data (one utterance per line)
>     output_file: Path to save the trained models (JSON format)
> """
> 
4a14,62
> import argparse
> from collections import defaultdict, Counter
> import sys
> 
> 
> def read_data(filepath):
>     """
>     Read training data from file.
>     Returns a list of utterances, where each utterance is a list of phonemes.
>     """
>     utterances = []
>     with open(filepath, 'r', encoding='utf-8') as f:
>         for line in f:
>             line = line.strip()
>             if line:
>                 # Split the line into phonemes
>                 phonemes = line.split()
>                 if phonemes:
>                     utterances.append(phonemes)
>     return utterances
> 
> 
> def add_boundary_markers(utterances):
>     """
>     Add begin-of-utterance (<s>) and end-of-utterance (</s>) markers.
>     Returns modified utterances with boundary markers.
>     """
>     marked_utterances = []
>     for utterance in utterances:
>         # Add <s> at the beginning and </s> at the end
>         marked = ['<s>'] + utterance + ['</s>']
>         marked_utterances.append(marked)
>     return marked_utterances
> 
> 
> def train_unigram(utterances):
>     """
>     Train a unigram model.
>     Returns a dictionary of unigram counts.
>     """
>     unigram_counts = Counter()
>     
>     for utterance in utterances:
>         for phoneme in utterance:
>             # Skip boundary markers for unigram
>             if phoneme not in ['<s>', '</s>']:
>                 unigram_counts[phoneme] += 1
>     
>     return dict(unigram_counts)
6,42d63
< # Input args for console input
< argparser = argparse.ArgumentParser()
< argparser.add_argument("input_file", type=str, help="Path to the input file")
< argparser.add_argument("output_file", type=str, help="Path to the output file")
< 
< # Zip technique taken from - https://stackoverflow.com/questions/13423919/computing-n-grams-using-python
< def find_ngrams(input_list, n):
<     ngram = []
<     for line in input_list:
<         if n == 1:
<             line = line.split()
<         elif n == 2:
<             line = ['<s>'] + line.split() + ['</s>']
<         elif n == 3:
<             line = ['<s>', '<s>'] + line.split() + ['</s>', '</s>']
<         ngram.extend(zip(*[line[i:] for i in range(n)]))
<     return Counter(ngram)
< 
< 
< # Convert the zip to counter, then to json
< def counter_to_normalized_json(ngram_count, n, context_counts, lap_smoothing=False, vocab_size=0):
<     result = {}
<     # Logic for normalization
<     if not lap_smoothing:
<         for ngram, count in ngram_count.items():
<             if n == 1:
<                 total = sum(ngram_count.values())   # .total not on lab machine
<                 result[' '.join(ngram)] = count / total
<             else :
<                 if n == 2: context = ngram[0:1]
<                 elif n == 3: context = ngram[0:2]
<                 result[' '.join(ngram)] = count / context_counts[' '.join(context)]
<     else:   # Laplace smoothing
<         for ngram, count in ngram_count.items():
<             if n == 2: context = ngram[0:1]
<             elif n == 3: context = ngram[0:2]
<             result[' '.join(ngram)] = (count + 1) / (context_counts[' '.join(context)] + vocab_size)
43a65,85
> def train_bigram(utterances):
>     """
>     Train a bigram model.
>     Returns a dictionary of bigram counts and context counts.
>     """
>     bigram_counts = defaultdict(Counter)
>     context_counts = Counter()
>     
>     for utterance in utterances:
>         for i in range(len(utterance) - 1):
>             context = utterance[i]
>             word = utterance[i + 1]
>             bigram_counts[context][word] += 1
>             context_counts[context] += 1
>     
>     # Convert defaultdict to regular dict for JSON serialization
>     result = {
>         'bigrams': {k: dict(v) for k, v in bigram_counts.items()},
>         'contexts': dict(context_counts)
>     }
>     
47,86c89,113
< def main():
<     args = argparser.parse_args()
<     train_file_path = Path(args.input_file)
<     model_output_path = Path(args.output_file)
< 
<     with open(train_file_path, 'r') as file:
<         lines = file.read().strip().split('\n')
< 
<         # Get the "context" (first 1/2 words in bi/trigram)
<         context_counts = Counter()
<         for ngram, count in find_ngrams(lines, 2).items():
<             context = ngram[0:1]    # The first element
<             context_counts[' '.join(context)] += count    # Track: * | con1 con2
< 
<         # Get the "context" (first 1/2 words in bi/trigram)
<         for ngram, count in find_ngrams(lines, 3).items():
<             context = ngram[0:2]    # The first two elements
<             context_counts[' '.join(context)] += count    # Track: * | con1 con2
< 
<         # Unigram - just count and normalize the counts
<         unigram_json = counter_to_normalized_json(find_ngrams(lines, 1), 1, context_counts, lap_smoothing=False)
<         vocab_size = len(unigram_json)
<         # Bigram
<         bigram_json = counter_to_normalized_json(find_ngrams(lines, 2), 2, context_counts, lap_smoothing=False)
<         # Trigram
<         trigram_json = counter_to_normalized_json(find_ngrams(lines, 3), 3, context_counts, lap_smoothing=False)
<         # Bigram + Laplace
<         bigram_laplace_json = counter_to_normalized_json(find_ngrams(lines, 2), 2, context_counts, lap_smoothing=True, vocab_size=vocab_size)
<         # Trigram + Laplace
<         trigram_laplace_json = counter_to_normalized_json(find_ngrams(lines, 3), 3, context_counts, lap_smoothing=True, vocab_size=vocab_size)
< 
<         # Final json with all 5 models
<         final_output = {
<             "vocab_size": vocab_size,     # For laplace smoothing TODO vocab of uni or bri/tri?
<             "context": context_counts,
<             "unigram": unigram_json,
<             "bigram": bigram_json,
<             "trigram": trigram_json,
<             "bigram_laplace": bigram_laplace_json,
<             "trigram_laplace": trigram_laplace_json,
---
> def train_trigram(utterances):
>     """
>     Train a trigram model.
>     Returns a dictionary of trigram counts and bigram context counts.
>     """
>     trigram_counts = defaultdict(lambda: defaultdict(Counter))
>     bigram_context_counts = defaultdict(Counter)
>     
>     for utterance in utterances:
>         for i in range(len(utterance) - 2):
>             w1 = utterance[i]
>             w2 = utterance[i + 1]
>             w3 = utterance[i + 2]
>             
>             trigram_counts[w1][w2][w3] += 1
>             bigram_context_counts[w1][w2] += 1
>     
>     # Convert nested defaultdicts to regular dicts for JSON serialization
>     result = {
>         'trigrams': {
>             k1: {k2: dict(v2) for k2, v2 in v1.items()}
>             for k1, v1 in trigram_counts.items()
>         },
>         'bigram_contexts': {
>             k: dict(v) for k, v in bigram_context_counts.items()
87a115,180
>     }
>     
>     return result
> 
> 
> def get_vocabulary(utterances):
>     """
>     Get the vocabulary (set of unique phonemes) from the utterances.
>     """
>     vocab = set()
>     for utterance in utterances:
>         for phoneme in utterance:
>             if phoneme not in ['<s>', '</s>']:
>                 vocab.add(phoneme)
>     return sorted(list(vocab))
> 
> 
> def main():
>     parser = argparse.ArgumentParser(
>         description='Train n-gram language models from phonetic sequences'
>     )
>     parser.add_argument('input_file', help='Path to the training data file')
>     parser.add_argument('output_file', help='Path to save the trained models (JSON)')
>     
>     args = parser.parse_args()
>     
>     print(f"Reading training data from {args.input_file}...")
>     utterances = read_data(args.input_file)
>     print(f"Loaded {len(utterances)} utterances")
>     
>     # Add boundary markers
>     print("Adding boundary markers...")
>     marked_utterances = add_boundary_markers(utterances)
>     
>     # Get vocabulary
>     vocab = get_vocabulary(marked_utterances)
>     print(f"Vocabulary size: {len(vocab)}")
>     
>     # Train models
>     print("Training unigram model...")
>     unigram_model = train_unigram(marked_utterances)
>     
>     print("Training bigram model...")
>     bigram_model = train_bigram(marked_utterances)
>     
>     print("Training trigram model...")
>     trigram_model = train_trigram(marked_utterances)
>     
>     # Prepare output
>     models = {
>         'unigram': unigram_model,
>         'bigram': bigram_model,
>         'trigram': trigram_model,
>         'vocabulary': vocab,
>         'num_utterances': len(utterances)
>     }
>     
>     # Save to JSON
>     print(f"Saving models to {args.output_file}...")
>     with open(args.output_file, 'w', encoding='utf-8') as f:
>         json.dump(models, f, indent=2)
>     
>     print("Training complete!")
>     print(f"Unigram types: {len(unigram_model)}")
>     print(f"Bigram types: {sum(len(v) for v in bigram_model['bigrams'].values())}")
>     print(f"Trigram types: {sum(sum(len(v2) for v2 in v1.values()) for v1 in trigram_model['trigrams'].values())}")
89,90d181
<         with open(model_output_path, "w") as output:
<             json.dump(final_output, output, indent=4)
95,96c186,199
< import argparse
< from pathlib import Path
---
> #!/usr/bin/env python3
> """
> Evaluate n-gram language models by computing perplexity.
> 
> Usage:
>     python3 src/eval.py <model_type> <model_file> <data_file> [--laplace]
> 
> Arguments:
>     model_type: Type of model (unigram, bigram, or trigram)
>     model_file: Path to the trained models (JSON format)
>     data_file: Path to the data file for evaluation
>     --laplace: (optional) Use Laplace (add-one) smoothing
> """
> 
97a201
> import argparse
98a203
> import sys
100,105d204
< # Input args for console input
< argparser = argparse.ArgumentParser()
< argparser.add_argument("model_type", type=str, choices=["unigram", "bigram", "trigram"], help="Which type of model to use")
< argparser.add_argument("input_model", type=str, help="Path to the model")
< argparser.add_argument("input_data", type=str, help="Path to the evaluation set.")
< argparser.add_argument("--laplace", action='store_true', help="Use laplace smoothing bigram/trigram model")
106a206,238
> def read_data(filepath):
>     """
>     Read evaluation data from file.
>     Returns a list of utterances, where each utterance is a list of phonemes.
>     """
>     utterances = []
>     with open(filepath, 'r', encoding='utf-8') as f:
>         for line in f:
>             line = line.strip()
>             if line:
>                 phonemes = line.split()
>                 if phonemes:
>                     utterances.append(phonemes)
>     return utterances
> 
> 
> def add_boundary_markers(utterances):
>     """
>     Add begin-of-utterance (<s>) and end-of-utterance (</s>) markers.
>     """
>     marked_utterances = []
>     for utterance in utterances:
>         marked = ['<s>'] + utterance + ['</s>']
>         marked_utterances.append(marked)
>     return marked_utterances
> 
> 
> def compute_unigram_perplexity(model_data, utterances):
>     """
>     Compute perplexity using unigram model (no smoothing for unigrams).
>     """
>     unigram_counts = model_data['unigram']
>     total_count = sum(unigram_counts.values())
108,125c240,241
< def main():
<     args = argparser.parse_args()
<     model_type = args.model_type
<     model_path = Path(args.input_model)
<     data_path = Path(args.input_data)
<     laplace = args.laplace
< 
<     if model_type == "unigram" and laplace: raise Exception("Cannot use laplace smoothing with unigram")
< 
<     if laplace:
<         model_type = model_type + "_laplace"
<     
<     # Load the model into memory
<     with open(model_path, 'r') as model_json:
<         data = json.load(model_json)
<         vocab_size = data["vocab_size"]
<         context_count = data["context"]
<         model = data[model_type]
---
>     log_prob_sum = 0
>     N = 0  # Total number of tokens
127,128c243,275
<     with open(data_path) as file:        
<         test_input = file.read().strip().split('\n')
---
>     for utterance in utterances:
>         for phoneme in utterance:
>             # Skip boundary markers
>             if phoneme in ['<s>', '</s>']:
>                 continue
> 
>             N += 1
>             count = unigram_counts.get(phoneme, 0)
> 
>             if count == 0:
>                 # Handle OOV: assign a very small probability
>                 # Use 1 / (total_count + 1) as a simple approach
>                 prob = 1 / (total_count + 1)
>             else:
>                 prob = count / total_count
> 
>             log_prob_sum += math.log2(prob)
> 
>     if N == 0:
>         return float('inf')
> 
>     # Perplexity = 2^(-1/N * sum(log2(P(w_i))))
>     perplexity = 2 ** (-log_prob_sum / N)
>     return perplexity
> 
> 
> def compute_bigram_perplexity(model_data, utterances, use_laplace=False):
>     """
>     Compute perplexity using bigram model.
>     """
>     bigram_counts = model_data['bigram']['bigrams']
>     context_counts = model_data['bigram']['contexts']
>     vocab_size = len(model_data['vocabulary'])
131,167c278,297
<     token_count = 0
<     unseen_count = 0
<     test_count = 0
<     for sentence in test_input:
<         n = 1
< 
<         # Append the start sentence tags
<         sentence = sentence.split(' ')
<         if "bigram" in model_type:
<             n = 2
<             sentence = ["<s>"] + sentence + ["</s>"]
<         elif "trigram" in model_type:
<             n = 3
<             sentence = ["<s>", "<s>"] + sentence + ["</s>", "</s>"]
< 
<         ngram = (zip(*[sentence[i:] for i in range(n)]))
<         for token in ngram:
<             test_count += 1
<             value = model.get(' '.join(token), None)
< 
<             if value is None:
<                 unseen_count += 1
< 
<                 if laplace:
<                     # Bi/Trigram laplace smoothing
<                     if n == 2: context = token[0:1]
<                     elif n == 3: context = token[0:2]
<                     context_result = context_count.get(context, 0)
<                     
<                     # Can try rolling back from trigram to bigram. 
<                     # Unigram isn't stored in context so I can't use that
<                     if n == 3:
<                         if context_result == 0: context_result = context_count.get(context[0], 0)
<                         if context_result == 0: context_result = context_count.get(context[1], 0)
<                         
<                     # Default val for missing tokens in model
<                     value = 1 / (context_result + vocab_size)
---
>     N = 0  # Total number of bigrams
> 
>     for utterance in utterances:
>         for i in range(len(utterance) - 1):
>             context = utterance[i]
>             word = utterance[i + 1]
>             N += 1
> 
>             if use_laplace:
>                 # Laplace smoothing: P(w|c) = (count(c,w) + 1) / (count(c) + V)
>                 bigram_count = bigram_counts.get(context, {}).get(word, 0)
>                 context_count = context_counts.get(context, 0)
>                 prob = (bigram_count + 1) / (context_count + vocab_size)
>             else:
>                 # No smoothing
>                 context_count = context_counts.get(context, 0)
> 
>                 if context_count == 0:
>                     # Unseen context: use a very small probability
>                     prob = 1e-10
169,179c299,402
<                     # value = 1 / total_vocab_size => (vocab_size ^ -ngram_dim)
<                     value = math.pow(vocab_size, -n)
<                     
<             # Sum log probabilites
<             log_prob_sum += math.log(value)
<             token_count += 1
< 
<     # Log perplexity
<     perplexity = math.exp(-1/token_count * log_prob_sum)
<     print(f'Final perplexity for {model_type}: {perplexity}')
<     print(f'Unseen tokens: {unseen_count} out of {test_count}')
---
>                     bigram_count = bigram_counts.get(context, {}).get(word, 0)
>                     if bigram_count == 0:
>                         # Unseen bigram: use a very small probability
>                         prob = 1e-10
>                     else:
>                         prob = bigram_count / context_count
> 
>             log_prob_sum += math.log2(prob)
> 
>     if N == 0:
>         return float('inf')
> 
>     perplexity = 2 ** (-log_prob_sum / N)
>     return perplexity
> 
> 
> def compute_trigram_perplexity(model_data, utterances, use_laplace=False):
>     """
>     Compute perplexity using trigram model.
>     """
>     trigram_counts = model_data['trigram']['trigrams']
>     bigram_context_counts = model_data['trigram']['bigram_contexts']
>     vocab_size = len(model_data['vocabulary'])
> 
>     log_prob_sum = 0
>     N = 0  # Total number of trigrams
> 
>     for utterance in utterances:
>         for i in range(len(utterance) - 2):
>             w1 = utterance[i]
>             w2 = utterance[i + 1]
>             w3 = utterance[i + 2]
>             N += 1
> 
>             if use_laplace:
>                 # Laplace smoothing: P(w3|w1,w2) = (count(w1,w2,w3) + 1) / (count(w1,w2) + V)
>                 trigram_count = trigram_counts.get(w1, {}).get(w2, {}).get(w3, 0)
>                 bigram_count = bigram_context_counts.get(w1, {}).get(w2, 0)
>                 prob = (trigram_count + 1) / (bigram_count + vocab_size)
>             else:
>                 # No smoothing
>                 bigram_count = bigram_context_counts.get(w1, {}).get(w2, 0)
> 
>                 if bigram_count == 0:
>                     # Unseen context: use a very small probability
>                     prob = 1e-10
>                 else:
>                     trigram_count = trigram_counts.get(w1, {}).get(w2, {}).get(w3, 0)
>                     if trigram_count == 0:
>                         # Unseen trigram: use a very small probability
>                         prob = 1e-10
>                     else:
>                         prob = trigram_count / bigram_count
> 
>             log_prob_sum += math.log2(prob)
> 
>     if N == 0:
>         return float('inf')
> 
>     perplexity = 2 ** (-log_prob_sum / N)
>     return perplexity
> 
> 
> def main():
>     parser = argparse.ArgumentParser(
>         description='Evaluate n-gram language models'
>     )
>     parser.add_argument('model_type',
>                        choices=['unigram', 'bigram', 'trigram'],
>                        help='Type of model to evaluate')
>     parser.add_argument('model_file',
>                        help='Path to the trained models (JSON)')
>     parser.add_argument('data_file',
>                        help='Path to the evaluation data file')
>     parser.add_argument('--laplace',
>                        action='store_true',
>                        help='Use Laplace (add-one) smoothing')
> 
>     args = parser.parse_args()
> 
>     # Validate: unigram with laplace is invalid
>     if args.model_type == 'unigram' and args.laplace:
>         raise ValueError("Laplace smoothing is not applicable to unigram models")
> 
>     # Load models
>     with open(args.model_file, 'r', encoding='utf-8') as f:
>         model_data = json.load(f)
> 
>     # Read evaluation data
>     utterances = read_data(args.data_file)
> 
>     # Add boundary markers
>     marked_utterances = add_boundary_markers(utterances)
> 
>     # Compute perplexity based on model type
>     if args.model_type == 'unigram':
>         perplexity = compute_unigram_perplexity(model_data, marked_utterances)
>     elif args.model_type == 'bigram':
>         perplexity = compute_bigram_perplexity(model_data, marked_utterances, args.laplace)
>     elif args.model_type == 'trigram':
>         perplexity = compute_trigram_perplexity(model_data, marked_utterances, args.laplace)
> 
>     # Print perplexity
>     print(f"{perplexity:.2f}")
```

## Discussion

### What is most similar?

```
Both functions end up computing the perplexity of the n-grams with the same formula (lapance smoothing and not). Calculating the log perplexities is almost identical, hence our similar perplexity results between implementations.
They follow the same pattern of sliding over the tokens, getting the model and context counts which were computer earlier, and computes the running log perplexity sum. Variable names are close to mine, and coding style is fairly similar to mine.
Fetching the arguments and checking for which ngram to run is also very similar.
```

### What is most different?

```
The LLM split up the evaluation of each n-gram into multiple functions, while I kept all the logic in main(). This added tons of repeated lines that arent' present in my function.
The LLM uses a default perplexity of 1e-10 for missing ngrams, I attempt backoff (from tri to bi) first, then default to a perplexity of 1 / (vocab_size) if no ngram was found.
The model keys are also queried differently. I used a space separated string of tokens, while LLM uses the default python conversion from list to json.
Finally, I took the sum log perplexities to the power of e, not to the power of 2 like the LLM.
```
