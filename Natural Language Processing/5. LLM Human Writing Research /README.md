## Assignment 3 URL

Complete your assignment 3 URL:

https://github.com/CMPUT-461-561/f25-asn3-Madrobot9182

----

## Choice of LLM to compare against:

```
Claude
```

### Brief comments

```
While both LLM wrote way more lines then my solution, Clade's names variables and structurs code more similar over Chatgpt. Claude also used less functions, which to be clear is still like triple the number of functions I had, but better than CHatGPT's quadruple lol. 
```

----

## "Copy-and-paste" execution instructions to run the **LLM code** after your changes

```
% python3 src/claude/train.py data/tiny_training.txt model.json
% python3 src/claude/eval.py bigram model.json data/tiny_dev.txt --laplace
```

----

## YOUR Evaluation **copied** from what you submitted in assignment 3

| Model   | Smoothing  | Training set PPL | Dev set PPL |
| ------- | ---------- | ---------------- | ----------- |
| unigram | -          | 35.1395          | 35.0615     |
| bigram  | unsmoothed | 17.5752          | 17.6919     |
| bigram  | Laplace    | 16.0559          | 16.1058     |
| trigram | unsmoothed | 9.3504           | 9.8395      |
| trigram | Laplace    | 9.2017           | 9.5834      |

## LLM evaluation

Used `data/full_model_llm.json` agaisnt train.txt and text.txt

| Model   | Smoothing  | Training set PPL | Dev set PPL |
| ------- | ---------- | ---------------- | ----------- |
| unigram | -          | 34.86            | 34.78       |
| bigram  | unsmoothed | 15.73            | 15.80       |
| bigram  | Laplace    | 15.74            | 15.78       |
| trigram | unsmoothed | 7.37             | 8.06        |
| trigram | Laplace    | 7.61             | 7.78        |
