# Justification

## Performance

- Precision: 23.53%

- Recall: 69.06%

- True positive: 96 True negative: 152 False positive: 312 False negative 43

## Discussion

Clearly my grammar checker performed really poorly. It couldn't even reach a quarter precision, and any amount of gains in precision also cut into recall. 

1. For false positives (sentence flagged for error when it was okay): My grammar was probably just not complex enough to handle all the different forms of sentences. I stuck with basic patterns and relied on recursion to expand out to longer sentences, but these recusive patterns could easily miss some special joining pattern I didn't author.

2. For false negatives (sentence flagged as okay when there was an error): My grammar could have also been too liberal in matching sentences, even when they were wrong. I had many patterns, especially for proper nouns and verbs, like `NP XX` and `VP XX`, which would recurse with other non-terminals and could produced a sequence that was invalid.

This was expected to be honest. English is a very contextual, complex, and alive language, and trying to use a Context-**Free** Grammar to capture everything was never happening. There are so many rules to consider that this design could never build a perfect grammar checker. Even if you literally just write out <u>every possible</u> sentence structure in English, just consider that when new slang/phrases/words are added, that new modifications may be needed. Being the most alive language in the world, the grammar will forever be "outdated" -- ie the grammar has no way to generalize/learn the English language by itself. English can also be very ambugious, and without considering context, a perfect CFG would have to go beyond sentence structure and write out <u>every</u> possible sentence <u>ever written or can be written</u>. Obviously, whenever the language changes (alive language), so does the entire grammar, circuling back to the grammar being forever "outdated" as long as English is a relevant language.

Also, I probably went through about 5 "cycles", where within every "cycle" I would modify/add one rule at a time and test how it changed the percision/recall. Lost count of how many individual changes were made, but was probably close to triple digits worth of tweaking.
