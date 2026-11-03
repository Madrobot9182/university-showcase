"""
Utility functions for sanitizing and transforming the .cha files
"""
from pathlib import Path
from file_reader import write_to_file, get_clean_dir_path, get_transform_dir_path
from consts import CMUDICT, PUNCTUATION_PATTERN, EXCESS_SPACE_PATTERN, STRIP_SPACE_PATTERN, INLINE_REMOVE_PATTERN, \
    FORM_NON_PATTERN, LINKAGE_PATTERN, TAG_REPLACE_PATTERN, TAG_INPLACE_PATTEN, TAG_REMOVE_PATTERN


def cha_sanitizer(filepath: Path) -> Path:
    """Sanitize a file and return the output directory"""
    data = []
    with open(filepath, 'r') as file:
        for line in file.readlines():
            if '*' in line[0]:
                # A spoken tag, not extraneous. Count the words minus the leading tag
                word_counter.increment_original_count(len(line.split()) - 1)
                line = line_sanitizer(line)
                if len(line):
                    # Count the number of remaining cleaned words.
                    word_counter.increment_clean_count(len(line.split()))
                    word_counter.increment_transform_count(len(line.split()))
                    data.append(line)
    write_path = get_clean_dir_path(filepath)
    write_to_file(write_path, data)
    return write_path


def line_sanitizer(line: str) -> str:
    """Used in cha_sanitizer to clean up individual line.
       All patterns descriptions are compiled and document in consts.py"""
    # Sanitizer patterns - Inline word tags
    line = INLINE_REMOVE_PATTERN.sub('', line)
    line = FORM_NON_PATTERN.sub('', line)
    line = LINKAGE_PATTERN.sub(' ', line)

    # Sanitizer patterns - Bracket tags
    line = TAG_REPLACE_PATTERN.sub(r'\1', line)
    line = TAG_INPLACE_PATTEN.sub(r'\1\2\3', line)
    line = TAG_REMOVE_PATTERN.sub('', line)

    # Catch all punctuation and excess spacing
    line = PUNCTUATION_PATTERN.sub('', line)
    line = EXCESS_SPACE_PATTERN.sub(' ', line)
    line = STRIP_SPACE_PATTERN.sub('', line)
    return line


def cha_transformer(filepath: Path) -> None:
    """transform a clean file into another file"""
    data = []
    with open(filepath, 'r') as file:
        for line in file.readlines():
            line = line_transformer(line)
            if len(line):
                data.append(line)
    write_to_file(get_transform_dir_path(filepath), data)


def line_transformer(line: str) -> str:
    """Find the word matches in CMUDICT, otherwise try and approximate the result"""
    output = []
    for word in line.split():
        result = CMUDICT.get(word.upper())  # Try and fetch a dictionary entry
        if result:
            output.append(result)
        else:
            # Attempt to splice the string and find a close approximation
            backend_result = None  # We want to cache the biggest backend result
            splice_success = False
            for splice in range(len(word) - 1, 0, -1):
                frontend_result = CMUDICT.get(word[0:splice].upper())
                temp = CMUDICT.get(word[splice:-1].upper())
                backend_result = temp if temp is not None else backend_result  # Ignore misses

                # Either frontend is valid, or no match for frontend, but found a match in backend
                if frontend_result or (frontend_result is None and backend_result is not None):
                    splice_success = True
                    word_counter.increment_modify_success(1)

                    if frontend_result is None: frontend_result = ""
                    if backend_result is None: backend_result = ""
                    output.append(frontend_result + " " + backend_result)
                    break

            # If no match was found reject the word. Decrement the transformed words count
            if not splice_success:
                word_counter.increment_transform_count(-1)

    return ' '.join(output)  # Return the transformed sentence


class WordCounter:
    """For tracking how many words are processed. In a class so I can pass it through functions and files"""

    def __init__(self):
        self.original_count = 0
        self.clean_count = 0
        self.transform_count = 0
        self.modify_success = 0

    def increment_original_count(self, value):
        self.original_count += value

    def increment_clean_count(self, value):
        self.clean_count += value

    def increment_transform_count(self, value):
        self.transform_count += value

    def increment_modify_success(self, value):
        self.modify_success += value

    def get_original_clean_ratio(self):
        # Ratio of original words to clean words
        return round(100 * self.clean_count / self.original_count, 2)

    def get_clean_transform_ratio(self):
        # Ratio of successful clean words to transformed words
        return round(100 * self.transform_count / self.clean_count, 2)

    def get_modify_success_ratio(self):
        # Ratio of successful modifications to failures
        return round(100 * self.modify_success / (self.modify_success + (self.clean_count - self.transform_count)), 2)


word_counter = WordCounter()
