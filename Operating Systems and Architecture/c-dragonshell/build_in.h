#pragma once

#include "./constants.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t child_pid = 0;    // A global pid for any (one) child processes
pid_t bg_pid = 0;       // A global pid for any (one) active background process
pid_t sus_pid = 0;      // A global pid for any (one) suspended process

/**
 * @brief finds the next instance of a token in the argv array
 *
 * @param argv - A char* array with all the arguments
 * @param token - A null terminated token to look for
 *
 * @return int - the index of the token, returns -1 if not found
 */
int find_token(char** argv, char* token){
    size_t i = 0;
    while (argv[i]) {
        if (strcmp(token, argv[i]) == 0)
            return i;
        i++;
    }
    // Else there is no pipe token
    return -1;
}

/**
 * @brief finds all arrow tokens in argv, puts indexes into array
 *
 * @param argv - A char* array with all the arguments
 * @param token - A null terminated token to look for
 *
 * int* token - An array of token indexes. -1 for < | 1 for > | else 0
 */
void find_arrow_token(char** argv, int* tokens){

    size_t i = 0;
    while (argv[i]) {
        if (strcmp("<", argv[i]) == 0)
            tokens[i] = -1;
        else if (strcmp(">", argv[i]) == 0)
            tokens[i] = 1;
        else
            tokens[i] = 0;
        i++;
    }
}


/**
 * @brief function for the cd command
 *
 * @param path - the pathway to move to
 */
void cd (const char* path) {
    // Initialize new dir to the current directory
    char new_dir[LINE_LENGTH];
    getcwd(new_dir, LINE_LENGTH);

    if (path == NULL || path[0] == '\0') {
        // No arguments provided, return
        fprintf(stderr, "dragonshell: Expected argument to \"cd\"\n");
        return;
    }

    if (path[0] == '/') {
        // This is a absolute path
        chdir(path);
    } else {
        // This is a relative path, need to create new path string

        //Append the slash first
        int len = strlen(new_dir);
        new_dir[len] = '/';
        new_dir[len+1] = '\0';

        strcat(new_dir, path); //Append the new directory

        int result = chdir(new_dir);
        if (result == -1) {
            //fprintf(stderr, "No such file or directory\n");
            perror("dragonshell");
        }
    }
}


/**
 * @brief function for the pwd command
 *
 */
void pwd () {
    char buffer[LINE_LENGTH];
    getcwd(buffer, LINE_LENGTH);

    printf("%s", buffer);
    putc('\n', stdout);
}

/**
 * @brief function for the exit command. Terminates any running process and itself
 *
 * @return 0 when all processes are terminated successfully
 */
int exit_shell () {
    struct rusage r_child;
    int total_user_time = 0;
    int total_sys_time = 0;

    // First terminate background process
    if(bg_pid) {
        kill(bg_pid, SIGTERM);
        wait(&bg_pid);
    }

    // Then suspended process. Resume process manually then kill it for time
    if (sus_pid) {
        // Return to foreground and kill
        kill(sus_pid, SIGCONT);
        kill(sus_pid, SIGTERM);
        wait(&sus_pid);
    }

    // Then kill foreground if any
    if (child_pid){ // Don't kill dragonshell (pid=0) itself
        kill(child_pid, SIGTERM);
        wait(&child_pid);
    }
        // Get usage statistics
        getrusage(RUSAGE_CHILDREN,&r_child);
        total_user_time = r_child.ru_utime.tv_sec;
        total_sys_time = r_child.ru_stime.tv_sec;


    printf("User time: %d seconds\nSys time: %d seconds\n", total_user_time, total_sys_time);
    // To end the shell loop in main
    return 0;
}
