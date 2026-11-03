# Justification

Below is my justifications for the decisions I made. My first actions + assumptions I made was:

- Took 1-2 files from each file in data and skimmed through, copying "interesting" lines to study and test regex on later.
- Much of the formatting would be important for tasks where we care about the intentions and semantics of the speaker and text. However, since we're focused on "the likelihood of different sound sequences in English," phonetics, the majority of these special formattings can be stripped out in the sanitizing process.
  - Hence why I was more liberal in sanitizing symbols and why about 60% of the spoken text is stripped out. That way I could achieve very strong results in the transformation step later

# Sanitizing/Cleaning

Aftering filtering out header and meta information, I was left with just the spoken transcriptions which was filled with special formatting and tagging. To get started, I assumed that all .cha files followed the CHAT Transcription Manual by MacWhinney (specifically section 8-10), and wrote regex to detect the most common patterns

### General Filtering

- Removed all the header `@` and extraneous info lines `%`

- Removed content between the unicode symbol `\u0015 (NAK)`

- After all the special filters below are applied, performed a general punctuation filter to catch every type of non-letter character in the line.

### Words (Section 8 and 9)

- Form Markers (word`@`code): I deleted the whole chunk. If the word was normal but just tagged for an irrevelent reason (letter/syllables/singing/sigh language/meta-ling.), then I only removed the tag.
  
  - Many of these words are non-standard and wouldn't be found in the dictionary anyways, so it was easier to blanket remove these symbols
  - Tagged fillers/stop words like `&-um` and `&~cluck` only had the preceding `&*` removed. Since the end goal is to predict sound sequences, I felt like it was important to keep the umms intact

- Non/ommitted words like `xxx` and/or lines with only `0` are removed. 

- "Linkages (`_` between compound words)": not needed for our purpose, so I replace these with spaces

- "Shortenings (parenthesis around letters)": `(letter(s)` removed with letter(s) kept in place, as they represent parts of words that weren't spoken but intended nontheless.

### Scoped words (Section 10)

- Brackets tags with `[!= event] [!] [!!] [# time] [=? alt] [% comment] [?] [<] [>] [+<] [/] [//]` are just removed as they aren't important for the task.

- The bracket tag `[: replacement]`, will replace the (1) previous word with the replacement. Usually the target is babble or a non-standard word, so it'll be more likely the replacement appears in the dictionary.

- The bracket tag [= target] will just be expanded to the target word(s). This tag is used really inconsistently, but usually contains important enough infomation that I feel it should still be included
  
  - Ex. There are many examples of `0 [= context of what's happening]`. Removing all of these would strip a lot of good data.

- Words between `<text>`, the angle brackets were removed, as the text is valid, but the bracket use is irrevelent for this task.

# Transformation

With the assumption that 90%+ of the words left can be found in the dictionary, Most of the words are just a simple find and replace with the phonetic spelling. Some words do have alternative spellings, indicated by the brackets (#), which I simply ignore. I also ignore the entries for punctuation/non-words, as those were removed all during sanitization.

For the remaining 10%: if I couldn't find a direct match, I attempt to approximate the next "closest" sound:

- I splice the word from the end letter moving backwards, and see if either the shortened string (frontend) or the spliced letters (backend) have a match. 
  
  - Every match for the back-end is saved until a match for the front end is found, in which case I squish both together and output the result.

- Otherwise the word is discarded, and the success % is reduced

I believe that prioritizing the first half of a word will be more helpful for our downsteam task. Humans, me included, will probably recall the first few letters of a word first, and there is a cool phenomenon dubbed [Typoglycemia]([Transposed letter effect - Wikipedia](https://en.wikipedia.org/wiki/Transposed_letter_effect) which could possibly explain how.

# Performance

To help gague accuracy of how much data was actually processed, I tracked how many words were processed and kept within the file. 

- The first variable was simply the number of "words" in a spoken dialog tag line, which would include all the extra formatting and punctuation. Hence the massive reduction of words from the original to sanitized count. 
  
  - It's hard to estimate how much of said reduction was potential/valid words vs non-words, but I believe that I still achieved good accuracy nontheless.

- From sanitization, I also compared how much data was perserved with and without my earlier attempt at splicing a string. Below are two outputs, with the first being from a while ago when I was still making Regex expressions, to the current implementation:

```
# My old output, without attempting to modify for better transformation
644 data files found. Beginning content processing...
125749 unique entries found in the cmudict...
Done! Calculated stats below:
Number of spoken words originally in the files: 1648149
Number of words after sanitizing the files: 1351597 (82.01% preserved from original)
Number of words after transforming the data: 1021918 (75.61% preserved from sanitized)

# Current implementation, where I walk over the string to find an approximate match
...
Number of (spoken) words originally in the files:  1648149
Number of (spoken) words originally in the files:  1639697
Number of words after sanitizing the files:         999062 (60.93% preserved from original)
Number of words after transforming the data:        998766 (99.97% preserved from sanitized)
Number of words modified to find a transformation:   54271 (99.46% success rate)

```

# References

MacWhinney, B. (2000). The CHILDES Project: Tools for Analyzing Talk. 3rd Edition.
Mahwah, NJ: Lawrence Erlbaum Associates. https://talkbank.org/0info/manuals/CHAT.pdf

[Transposed letter effect - Wikipedia](https://en.wikipedia.org/wiki/Transposed_letter_effect)
