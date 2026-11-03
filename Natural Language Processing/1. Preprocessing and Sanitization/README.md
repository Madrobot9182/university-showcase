# README

### "Copy-and-paste" execution instructions

Run main.py to start the process:

```
python3 src/main.py
```

The output folders will be in `output/clean` and `output/transform`. I also wrote a simple script to delete all the output files, which can be run with:

```
python3 src/delete_output.py
```

### "Copy-and-paste" instructions for installing libraries and/or files

Only the python `re` and `path` modules are used. No external libraries needed.

### Resources and AI assistants used

1. The only time I used AI was figuring out how to track and increment word counts across different files. After strugging with python global variables, llama3 suggested using a class object, which worked.

### Complete output of your code on a lab machine

```
% ssh ry7@ohaton.cs.ualberta.ca 
ry7@ohaton.cs.ualberta.ca's password: 
Welcome to Ubuntu 20.04.6 LTS (GNU/Linux 5.15.0-139-generic x86_64)

Department of Computing Science
University of Alberta
... More lab machine stuff ...
Last login: Mon Sep 22 20:15:48 2025 from 50.65.111.28
ry7@ohaton:~>cd cmput461
ry7@ohaton:~/cmput461>python3 src/main.py 
644 data files found. Beginning content processing...
125749 unique entries found in the cmudict...
Done! Calculated stats below:
Number of (spoken) words originally in the files:  1639697
Number of words after sanitizing the files:         999062 (60.93% preserved from original)
Number of words after transforming the data:        998766 (99.97% preserved from sanitized)
Number of words modified to find a transformation:   54271 (99.46% success rate)
```
