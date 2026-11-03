import re
from pathlib import Path

# Assuming this file is in /ROOT/src/const.py
ROOT_PATH = Path(__file__).parent.parent
DATA_DIRECTORY = Path(ROOT_PATH, "data")
CLEAN_OUTPUT_DIR = Path(ROOT_PATH, "output/clean")
TRANSFORM_OUTPUT_DIR = Path(ROOT_PATH, "output/transform")
CMUDICT_PATH = Path(ROOT_PATH, "cmudict-0.7b")

# CMUDICT regex and hashmap initializer
CMUDICT_PATTERN = re.compile(r"""^(.*[^\(\d\)])     # Capture g1 the label, exclude secondary (#) lines
                                        \s\s(.*)$   # Capture g2 the actual pronunciations""", re.X)
CMUDICT = {}

# Sanitizer patterns - Inline word tags
INLINE_REMOVE_PATTERN = re.compile(r"""^\*\w+: |             # Leading *tag 
                                                \bxxx\b | \b0\b |   # xxx and 0 by itself
                                                &- | &~ | &+ | &= | # & symbols in front of some characteres
                                                \u0015.*\u0015 |    # Content between NAK symbols
                                                @(?:i|k|l|n|o|q|si|sl|sas|x)   # @tags to be stripped""", re.X)
FORM_NON_PATTERN = re.compile(r'\w+@(?!i|k|l|n|o|si|sl|sas|x)\w+')  # word@tag to be deleted completely
LINKAGE_PATTERN = re.compile(r'_')  # w_w to replace with spaces

# Sanitizer patterns - Bracket tags
TAG_REPLACE_PATTERN = re.compile(r'\w+\s\[:\s(\w+)\]')  # Replacement [: word] in capture g1
TAG_INPLACE_PATTEN = re.compile(r"""<(.*?)> |       # Just remove the brackets in: <content>
                                            \((.*?)\) |    # \(content\)   
                                            \[=\s(.*?)\]   # \[= content\]""", re.X)
TAG_REMOVE_PATTERN = re.compile(r"""\[ |                  # Bracket pattern
                                                (?!(?:[:=])) |   # Ignore \[: cont\] and \[= cont\] patterns
                                                (?:.*?)\]        # Matches every other bracket patterns""", re.X)

PUNCTUATION_PATTERN = re.compile(r'[!@#\$%\^&\*\(\)_\+\-={}\[\]\|;:\'\",\.\/<>?]')  # Catch all non-letter characters
EXCESS_SPACE_PATTERN = re.compile(r'\s{2,}')  # Two or more whitespace chars
STRIP_SPACE_PATTERN = re.compile(r'(^\s+)|(\s+$)')  # Leading/trailing whitespace
