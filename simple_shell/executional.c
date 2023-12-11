#include "shell.h"

void execute_command( const char *command) {
    pid_t child_pid = fork();

    if (child_pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0){
        // the Child processess
        execve(command, command, (char *) NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    } else {
        // the Parent processess
        wait(NULL);

    }
}