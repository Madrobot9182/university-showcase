# README

nltk isn't on the lab machine by default so I installed it with pip3 (the example used wget but that's less convenient) 

### "Copy-and-paste" execution instructions

```python
python3 src/main.py data/train.tsv grammars/real.cfg output/output.tsv
```

### "Copy-and-paste" instructions for installing libraries and/or files

On the lab machine, install the `nltk` library with

```
pip3 install nltk
```

### Resources and AI assistants used

1. While researching about English grammar, I used Claude to help research grammar rules, the more niche non-terminals in Penn Treebank, and example sentences. Especially when I couldn't figure out why a sentence was "wrong" or "right" and felt like I was going crazy. 
2. The rest of my research was from various online articles, other univeristies slides, and Wikipedia.

### Complete output of your code on a lab machine

```
% ssh <lab machine>
ry7@ohaton:~/cmput461/f25-asn2-Madrobot9182>pip3 install nltk
Collecting nltk
  Downloading nltk-3.9.1-py3-none-any.whl (1.5 MB)
     |████████████████████████████████| 1.5 MB 9.5 MB/s 
Collecting regex>=2021.8.3
  Downloading regex-2024.11.6-cp38-cp38-manylinux_2_17_x86_64.manylinux2014_x86_64.whl (785 kB)
     |████████████████████████████████| 785 kB 112.5 MB/s 
Collecting joblib
  Downloading joblib-1.4.2-py3-none-any.whl (301 kB)
     |████████████████████████████████| 301 kB 90.4 MB/s 
Requirement already satisfied: click in /usr/lib/python3/dist-packages (from nltk) (7.0)
Collecting tqdm
  Downloading tqdm-4.67.1-py3-none-any.whl (78 kB)
     |████████████████████████████████| 78 kB 7.0 MB/s 
Installing collected packages: regex, joblib, tqdm, nltk
  WARNING: The script tqdm is installed in '/cshome/ry7/.local/bin' which is not on PATH.
  Consider adding this directory to PATH or, if you prefer to suppress this warning, use --no-warn-script-location.
  WARNING: The script nltk is installed in '/cshome/ry7/.local/bin' which is not on PATH.
  Consider adding this directory to PATH or, if you prefer to suppress this warning, use --no-warn-script-location.
Successfully installed joblib-1.4.2 nltk-3.9.1 regex-2024.11.6 tqdm-4.67.1
ry7@ohaton:~/cmput461/f25-asn2-Madrobot9182>python3 src/main.py data/train.tsv grammars/real.cfg output/output.tsv
True positive: 96 True negative: 152
False positive: 312 False negative 43
Precision: 0.23529411764705882
Recall: 0.6906474820143885
ry7@ohaton:~/cmput461/f25-asn2-Madrobot9182>
```