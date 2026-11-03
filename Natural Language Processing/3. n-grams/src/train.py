import argparse
from pathlib import Path
from collections import Counter
import json

# Input args for console input
argparser = argparse.ArgumentParser()
argparser.add_argument("input_file", type=str, help="Path to the input file")
argparser.add_argument("output_file", type=str, help="Path to the output file")

# Zip technique taken from - https://stackoverflow.com/questions/13423919/computing-n-grams-using-python
def find_ngrams(input_list, n):
    ngram = []
    for line in input_list:
        if n == 1:
            line = line.split()
        elif n == 2:
            line = ['<s>'] + line.split() + ['</s>']
        elif n == 3:
            line = ['<s>', '<s>'] + line.split() + ['</s>', '</s>']
        ngram.extend(zip(*[line[i:] for i in range(n)]))
    return Counter(ngram)


# Convert the zip to counter, then to json
def counter_to_normalized_json(ngram_count, n, context_counts, lap_smoothing=False, vocab_size=0):
    result = {}
    # Logic for normalization
    if not lap_smoothing:
        for ngram, count in ngram_count.items():
            if n == 1:
                total = sum(ngram_count.values())   # .total not on lab machine
                result[' '.join(ngram)] = count / total
            else :
                if n == 2: context = ngram[0:1]
                elif n == 3: context = ngram[0:2]
                result[' '.join(ngram)] = count / context_counts[' '.join(context)]
    else:   # Laplace smoothing
        for ngram, count in ngram_count.items():
            if n == 2: context = ngram[0:1]
            elif n == 3: context = ngram[0:2]
            result[' '.join(ngram)] = (count + 1) / (context_counts[' '.join(context)] + vocab_size)
    
    return result


def main():
    args = argparser.parse_args()
    train_file_path = Path(args.input_file)
    model_output_path = Path(args.output_file)

    with open(train_file_path, 'r') as file:
        lines = file.read().strip().split('\n')

        # Get the "context" (first 1/2 words in bi/trigram)
        context_counts = Counter()
        for ngram, count in find_ngrams(lines, 2).items():
            context = ngram[0:1]    # The first element
            context_counts[' '.join(context)] += count    # Track: * | con1 con2
        
        # Get the "context" (first 1/2 words in bi/trigram)
        for ngram, count in find_ngrams(lines, 3).items():
            context = ngram[0:2]    # The first two elements
            context_counts[' '.join(context)] += count    # Track: * | con1 con2

        # Unigram - just count and normalize the counts
        unigram_json = counter_to_normalized_json(find_ngrams(lines, 1), 1, context_counts, lap_smoothing=False)
        vocab_size = len(unigram_json)
        # Bigram
        bigram_json = counter_to_normalized_json(find_ngrams(lines, 2), 2, context_counts, lap_smoothing=False)
        # Trigram
        trigram_json = counter_to_normalized_json(find_ngrams(lines, 3), 3, context_counts, lap_smoothing=False)
        # Bigram + Laplace
        bigram_laplace_json = counter_to_normalized_json(find_ngrams(lines, 2), 2, context_counts, lap_smoothing=True, vocab_size=vocab_size)
        # Trigram + Laplace
        trigram_laplace_json = counter_to_normalized_json(find_ngrams(lines, 3), 3, context_counts, lap_smoothing=True, vocab_size=vocab_size)

        # Final json with all 5 models
        final_output = {
            "vocab_size": vocab_size,     # For laplace smoothing TODO vocab of uni or bri/tri?
            "context": context_counts,
            "unigram": unigram_json,
            "bigram": bigram_json,
            "trigram": trigram_json,
            "bigram_laplace": bigram_laplace_json,
            "trigram_laplace": trigram_laplace_json,
        }
    
        with open(model_output_path, "w") as output:
            json.dump(final_output, output, indent=4)

if __name__ == '__main__':
    main()
