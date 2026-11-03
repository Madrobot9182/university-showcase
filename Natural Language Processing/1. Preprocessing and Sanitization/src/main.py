"""Main function to clean and process .cha files"""
from file_reader import get_all_file_paths, read_cmudict_dict
from file_transformer import cha_sanitizer, cha_transformer, word_counter
from consts import DATA_DIRECTORY


def main():
    """Fetch all .cha files, make clean and transform files"""
    filepaths = get_all_file_paths(DATA_DIRECTORY)
    print(f'{len(filepaths)} data files found. Beginning content processing...')
    cmudict_count = read_cmudict_dict()     # To initialize the CMUDICT for transforming
    print(f'{cmudict_count} unique entries found in the cmudict...')

    for filename in filepaths:
        # 1. Open and Sanitize the file, write to clean
        clean_filename = cha_sanitizer(filename)
        # 2. Open (from clean) and Transform it, write to transform
        cha_transformer(clean_filename)

    print("Done! Calculated stats below:")
    print(f'Number of (spoken) words originally in the files:  {word_counter.original_count:>7}')
    print(f'Number of words after sanitizing the files:        {word_counter.clean_count:>7} ({word_counter.get_original_clean_ratio()}% preserved from original)')
    print(f'Number of words after transforming the data:       {word_counter.transform_count:>7} ({word_counter.get_clean_transform_ratio()}% preserved from sanitized)')
    print(f'Number of words modified to find a transformation: {word_counter.modify_success:>7} ({word_counter.get_modify_success_ratio()}% success rate)')

if __name__ == "__main__":
    main()
