import argparse
from pathlib import Path
import json
import math

# Input args for console input
argparser = argparse.ArgumentParser()
argparser.add_argument("model_type", type=str, choices=["unigram", "bigram", "trigram"], help="Which type of model to use")
argparser.add_argument("input_model", type=str, help="Path to the model")
argparser.add_argument("input_data", type=str, help="Path to the evaluation set.")
argparser.add_argument("--laplace", action='store_true', help="Use laplace smoothing bigram/trigram model")


def main():
    args = argparser.parse_args()
    model_type = args.model_type
    model_path = Path(args.input_model)
    data_path = Path(args.input_data)
    laplace = args.laplace

    if model_type == "unigram" and laplace: raise Exception("Cannot use laplace smoothing with unigram")

    if laplace:
        model_type = model_type + "_laplace"
    
    # Load the model into memory
    with open(model_path, 'r') as model_json:
        data = json.load(model_json)
        vocab_size = data["vocab_size"]
        context_count = data["context"]
        model = data[model_type]

    with open(data_path) as file:        
        test_input = file.read().strip().split('\n')

    log_prob_sum = 0
    token_count = 0
    unseen_count = 0
    test_count = 0
    for sentence in test_input:
        n = 1

        # Append the start sentence tags
        sentence = sentence.split(' ')
        if "bigram" in model_type:
            n = 2
            sentence = ["<s>"] + sentence + ["</s>"]
        elif "trigram" in model_type:
            n = 3
            sentence = ["<s>", "<s>"] + sentence + ["</s>", "</s>"]

        ngram = (zip(*[sentence[i:] for i in range(n)]))
        for token in ngram:
            test_count += 1
            value = model.get(' '.join(token), None)

            if value is None:
                unseen_count += 1

                if laplace:
                    # Bi/Trigram laplace smoothing
                    if n == 2: context = token[0:1]
                    elif n == 3: context = token[0:2]
                    context_result = context_count.get(context, 0)
                    
                    # Can try rolling back from trigram to bigram. 
                    # Unigram isn't stored in context so I can't use that
                    if n == 3:
                        if context_result == 0: context_result = context_count.get(context[0], 0)
                        if context_result == 0: context_result = context_count.get(context[1], 0)
                        
                    # Default val for missing tokens in model
                    value = 1 / (context_result + vocab_size)
                else:
                    # value = 1 / total_vocab_size => (vocab_size ^ -ngram_dim)
                    value = math.pow(vocab_size, -n)
                    
            # Sum log probabilites
            log_prob_sum += math.log(value)
            token_count += 1

    # Log perplexity
    perplexity = math.exp(-1/token_count * log_prob_sum)
    print(f'Final perplexity for {model_type}: {perplexity}')
    print(f'Unseen tokens: {unseen_count} out of {test_count}')


if __name__ == '__main__':
    main()
