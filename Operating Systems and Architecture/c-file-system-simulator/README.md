# # Design Choices

My function is split into two files.

- main.c handles reading the input file, parsing the result, and calling the specified function. This was to keep the logic separate from the file system itself

- fs-sim.c actually implements all the functions. It also contains all global variables like the buffers, reference to the current working directory, and open file descriptor.

  - My only regret is not collecting some functions into a helper method. It would have cleaned up the otherwise long fs-sim and potentially made debugging faster

  - fs_delete was by far the hardest function, because I wanted to make it recursive. It initially used more global variables and overly complicated logic until I switch to static variables. It took a few rewrites until to worked correctly, but totally worth it as that's the coolest function I made!

- Maybe not revelent, but I re-drew the lab manual on a whiteboard on my desk, with a visual summary of what the file system looked like. Worked amazingly for when I needed to quicky reference which bit in the inode does what, or what the max lengths of the inode vs data blocks were.

# System Calls

- In main, I used fopen to open the input file. It makes it easy to read line by line

- In fs-sim, I decided to follow what the lab manual suggested and stuck with lseek, open, close, read, write. 

  - Open and close are only called during mount and termination (and the sys call access to check if the disk/file exist)

  - lseek is usually used alongside read when I needed to read data, and write whenever I needed to write data. This includes fs_resize, fs_defrag.

# Testing

Testing was done slowly and carefully. 

- Whenever I make a new function I make some test examples and see if it behaves correctly

- Then I go though the provided test cases. If something goes wrong I track down which line(s) caused the issue and work backwards to find the defect

- Lots of print statements, and I wrote some print_bin which printed the bits of a uint, to help visualize the bits

- I also used ghex, a bit viewing tool like hexdump with a gui. Both worked, but I was already familar with ghex so I used that for viewing the disk

# Sources

All the man pages for the system calls.

where I learned to use the access command - [filesystems - What&#39;s the best way to check if a file exists in C? - Stack Overflow](https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c)

Made bit manuplation alot easier compared to my old jank method - [Modify a bit at a given position - GeeksforGeeks](https://www.geeksforgeeks.org/modify-bit-given-position/)

For fs_delete - [Static Variables in C - GeeksforGeeks](https://www.geeksforgeeks.org/static-variables-in-c/)
