from pathlib import Path
from random import shuffle
from collections import Counter

def main():
    data_dir = Path(Path(__file__).parent.parent, "data/original_transform")
    
    file_list = [file for file in data_dir.rglob("*.txt")]
    #vocab_counter = Counter()

    # Now read the contents of every file and compress them into 80% train 20% test
    shuffle(file_list)
    split_point = int(len(file_list) * 0.8)
    
    with open(Path(data_dir.parent, "train.txt"), 'w') as train:
        for file_name in file_list[0:split_point]:
            with open(file_name, 'r') as input_train:
                # Build the vocab set and write to training file
                for line in input_train.readlines():
                    line = line.strip()
                    #vocab_counter.update(line.split())
                    train.write(line + '\n')

    #print(vocab_counter)
    with open(Path(data_dir.parent, "test.txt"), 'w') as test:
        for file_name in file_list[split_point:]:
            with open(file_name, 'r') as input_test:
                for line in input_test.readlines():
                    test.write(line.strip() + '\n')
main()
