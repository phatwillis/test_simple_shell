#include "shell.h"

void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        if (execve(args[0], args) == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Forking failed
        perror("Error");
    } else {
        // Parent process
        // Wait for the child process to finish
        wait(NULL);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    
    for (write("Simple Shell 0.2> "); fgets(input, sizeof(input), stdin) != NULL;
     write("Simple Shell 0.2> ")) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Tokenize the input into command and arguments
        char *token = strtok(input, " ");
        char *args[MAX_INPUT_SIZE];
        int i = 0;

        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL; // Set the last element to NULL for execvp

        // Execute the command with arguments
        execute_command(args);
    }

    return (0);
}
