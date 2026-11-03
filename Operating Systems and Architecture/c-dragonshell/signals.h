#pragma once
#include <signal.h>
#include <stdio.h>

extern pid_t child_pid;
extern pid_t sus_pid;

/**
 * @brief SIGINT handler - interrupts the child_pid if exist
 *
 */
void sigint_handler(int signum){
    if (child_pid != 0){
        kill(child_pid, SIGINT);
        child_pid = 0; // reset child
    }
}

/**
 * @brief SIGTSTP handler - sets sus_pid and suspends child process
 *
 */
void sigtstp_handler(int signum){
    if (child_pid != 0){
        sus_pid = child_pid;
        kill(child_pid, SIGTSTP);
        child_pid = 0; // reset child
    }
}

/**
 * @brief Sets signal handlers for SIGINT and SIGSTP
 *
 */
void set_handlers() {
    struct sigaction sa_int;
    struct sigaction sa_stp;

    // Set entire struct to zero
    memset(&sa_int, 0, sizeof(sa_int));
    // Set flag to zero
    sa_int.sa_flags = 0;
    // Set the handler
    sa_int.sa_handler = sigint_handler;
    // Set the signal mask
    sigemptyset(&sa_int.sa_mask);
    //execute sigaction
    sigaction(SIGINT, &sa_int, NULL);

    // And again for SIGSTP
    memset(&sa_stp, 0, sizeof(sa_stp));
    sa_stp.sa_flags = 0;
    sa_stp.sa_handler = sigtstp_handler;
    sigemptyset(&sa_stp.sa_mask);
    sigaction(SIGTSTP, &sa_stp, NULL);
}
