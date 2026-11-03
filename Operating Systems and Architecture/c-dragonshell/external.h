#pragma once

#include "./constants.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern pid_t child_pid;
extern pid_t bg_pid;
/**
 * @brief Spawns a new process for a given command
 *
 * @param argv - An array of strings with all the arguments
 * @param adr_index - the index of & if it exist
 * @param print_bg_proc - a flag to indicate whether to print the PID message (N[0]/Y[1])
 *
 */
void run_process(char ** argv, int adr_index, int print_bg_proc) {
    // Try executing argv[0], pass in all other args (NULL the empty argv in main)
    // Fork child process, run execve, run wait for waiting on child process

    if (adr_index != -1) argv[adr_index] = NULL;

    pid_t pid = fork();
    if(pid == 0) { // child
        if(adr_index != -1)
            setpgid(0, 0); // send process to background

        execve(argv[0], argv, NULL);
        // exec doesn't return on success. If we reach here then it failed
        perror(argv[0]);
        _exit(1);
    } else if (pid > 0) { //parent
        if(adr_index == -1){
            // Foreground process
            child_pid = pid;
            wait(&pid); // Wait for the process to terminate
        }
        else {
            // Background process
            bg_pid = pid;

            // Let process run and return to shell
            if(print_bg_proc)
                printf("PID %d is sent to background\n", pid);
        }
    } else {
        perror("Fork error"); // This should never happen (probably)
    }
}


/**
 * @brief Executes and redirects file streams
 *
 * @param argv - A char* array with all the arguments
 * @param arr_index - an array of ( < == -1 || > == 1) indexes where the arrows are
 * @param adr_index - the index of & if it exist
 *
 */
void redirected_process(char ** argv, int* arr_index, int adr_index) {
    pid_t pid = fork();

    if (pid == 0) { // Child process
        // Input/Output redirection
        int in_fd = STDIN_FILENO;
        int out_fd = STDOUT_FILENO;

        // For every arrow, redirect outputs of programs to files
        for (int i = 0 ; i < MAX_ARGS; i++) {
            int direction = arr_index[i]; // -1 if < || 1 if > || 0 otherwise

            if (direction == -1) { // cmd < in file
                in_fd = open(argv[i+1], O_RDONLY);
                argv[i] = NULL;
                argv[i+1] = NULL;

            } else if (direction == 1) { // cmd > out file
                out_fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                argv[i] = NULL;
                argv[i+1] = NULL;
            }
        }
        // Redirect input (if needed)
        if(in_fd != STDIN_FILENO) {
            // duplicate the in stream file
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        // And again for out steam
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        // Execute the command
        run_process(argv, adr_index, 0);
        _exit(0); // Needs to be here to properly return (can't use exit(1))

    } else if (pid > 0){
        // Parent process, close everything and wait
        if(adr_index == -1)
            wait(&pid); // Wait for the process to terminate
        else
            fprintf(stdout, "PID %d is sent to background\n", pid);

    } else {
        perror("fork"); // This should never happen (probably)
    }
}



/**
 * @brief Runs two commands. Pipes the first into the second
 *
 * @param argv - A char* array with all the arguments
 * @param pipe_index - an index to which element of argv is the | token
 *
 */
void pipe_process(char** argv, int pipe_index) {
    int fd[2];

    // Split argv into two parts
    char** const arg_1 = argv;
    char** const arg_2 = &argv[pipe_index + 1];
    argv[pipe_index] = NULL;

    // Pipe and execute the process
    pipe(fd);

    if(fork() == 0){
        // First child process, send output
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execve(arg_1[0], arg_1, NULL);
        // Print error if failed
        perror(arg_1[0]);
        _exit(1);
    }  else { //parent
        // Parent process, fork again for second process and close all pipes
        if(fork() == 0){
            // Second child process, read from input
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            execve(arg_2[0], arg_2, NULL);
            // Print error if failed
            perror(arg_2[0]);
            _exit(1);
        } else {
            // close both ends of the pipe
            close(fd[0]);
            close(fd[1]);

            // Wait for both child processes to finish
            wait(NULL);
            wait(NULL);
        }
    }
}
