# # Design Choice


When designing my shell, I always write code with modularity and ease of expanding functionality. By following general OOP principles, seperating jobs to functions, I can reuse portions of code and improve readability. OOP also lets me test individual functions more easily.

With system calls I also need to constantly check online, the manual page, and create test functions. I use print statements and gradually comment out portions to debug code until it works (I couldnt get core dumps working so I couldnt get `gdb`working :(

# # System Calls


### Built in Commands

- cd - get current directory with `getcwd`, process the argument, then `chdir`.

- pwd - get current directory with `getcwd`and print it out

- exit - kills all processes, prints runtime, and returns 0 to end the loop in main
  
  - `kill(X_pid, SIGTERM)`to kill any (one) background, suspended, and foreground process
    
    - and `kill(sus_pid, SIGCONT)` to bring suspended process back so we can terminate it right after
  
  - `wait(&X_pid)`to ensure we wait for kill to execute
  
  - `rusage` to get runtime of all child processes.
    
    - `getrusage(RUSAGE_CHILDREN,&r_child)` to fill in the usage struct, then `ru_utime.tv_sec | su_utime.tv_sec`to get user and system time in seconds.
  
  - main will then free all memory and call `_exit(0)`.

### External Commands

- run_process -`fork()`and execute the process,
  
  -  call`execve`in child, use `setpgid(0,0)`to send a process to background.
  
  - parent either `wait(&pid)`for process to finish, or return to main (background)

- redirected_process - creates file streams to redirect input/output sterams
  
  - depending on the arrow, open a new stream with `open(arg, O_RDONLY)` for reading out, and `open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0644)`for writing in. 
    
    - if file doesnt exist then create it, else truncate into file
  
  - `dup2` and `close` the streams to duplicate STDIN and STDOUT to the created file streams. 
  
  - Finally resue the run_process function to execute the process but now with modified IN/OUT streams. Call `_exit(0)`in child for proper termination

- pipe_process - single layer process piping
  
  - `pipe(fd)`to create a pipe. We then `dup2()`the STDIN and STDOUT streams to pipe IO between commands.
  
  - Double`fork()`to execute the two commands. Then `close(fd[0/1)`the pipes and `wait()` for all child process to finish

### Signals

- Initalize two `sigaction` structs and set their parameters accordingly.
  
  - `sa_flags, sa_handler, sa_mask, sa_handler`
  
  - set the new handler so they will only kill child processes of the shell and not itself
  
  - call `sigaction()` to make the change. set_handlers() will be called in main



# Testing

What I mentioned earler. In addition I also made a simple program that loops for 10 seconds. I use this program to constantly test my process runner. I also made some test.txt files to test file redirections, and ran `/bin/w | /bin/wc -w`to test my piping function.

I have two extra make target: `make run` to compile and run the program in one line, and `make valgrind` to run my program under valgrind for mem leak checks.

Finally since I had developed everything on my personal linux machines (Arch btw~), I `scp`my project onto the university lab machines and perform a final round of tests.

# Sources

1. The majority of my references are the slides of Omid Ardakanian, Cmput 379 Fall 2024.
2. The man 2 pages from https://linux.die.net/man/2/ and https://man7.org/linux/man-pages/dir_section_2.html
3. https://stackoverflow.com/questions/10779020/sending-a-process-to-the-background-and-returning-control-to-my-shell
4. https://stackoverflow.com/questions/12902627/whats-the-difference-between-stdout-and-stdout-fileno
5. https://stackoverflow.com/questions/6792930/how-do-i-share-a-global-variable-between-c-files
