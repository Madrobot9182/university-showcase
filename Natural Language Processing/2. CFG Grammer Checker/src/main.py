import sys      # 1. train data 2. grammar path 3. output path
import nltk

def main():
    TP, FP, FN, TN = 0,0,0,0    # For error calc

    # Read CFG from file
    grammar = nltk.data.load(sys.argv[2])
    parser = nltk.ChartParser(grammar)
    # Open both training and output file
    with open(sys.argv[1], 'r') as data, open(sys.argv[3], 'w+') as output:
        lines = data.readlines()[1:]    # Skip top labels
        for line in lines:
            line = line.strip().split('\t')

            # Predict on the tokenized line
            token = line[3].split()
            # Either get first "tree" (from generator) or nothing
            first_tree = next(parser.parse(token), None)
            prediction = 0 if first_tree is not None else 1

            result_line = f'{line[0]}\t{line[1]}\t{prediction}\n'
            output.write(result_line)

            # Analyze the error
            truth = int(line[1])
            if truth == prediction and prediction == 1:
                TP += 1
            elif truth == prediction and prediction == 0:
                TN += 1
            elif truth != prediction and truth == 1:
                FN += 1
            elif truth != prediction and truth == 0:
                FP += 1

    # Print final calculations
    print(f"True positive: {TP} True negative: {TN}\nFalse positive: {FP} False negative {FN}")
    if TP or (FP and FN):    # Check for division by zero
        precision = TP / (TP + FP)
        recall = TP / (TP + FN)
        print(f'Precision: {precision}\nRecall: {recall}')
    else:
        print(f'Could not calculate precision and recall (potential division by zero)')

if __name__ == "__main__":
    main()
