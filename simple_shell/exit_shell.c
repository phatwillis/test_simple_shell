#include "shell.h"

void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        if (execve(args[0], args, NULL) == -1) {
            perror("Error");
            _exit(EXIT_FAILURE);
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

int is_command_exit(char *command) {
    return strcmp(command, "exit") == 0;
}

int main() {
    char input[MAX_INPUT_SIZE];
    
    for (write(STDOUT_FILENO, ":) ", 3); fgets(input, sizeof(input), stdin) != NULL; write(STDOUT_FILENO, ":) ", 3)) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Tokenize the input into command and arguments
        char *token = strtok(input, " ");
        char *args[MAX_INPUT_SIZE];
        int i = 0;

        while (token != NULL && i < MAX_INPUT_SIZE - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Set the last element to NULL for execve

        // Check if the command is the exit built-in
        if (is_command_exit(args[0])) {
            write(STDOUT_FILENO, "Exiting the shell\n", 18);
            break;
        }

        // Execute the command with arguments
        execute_command(args);
    }

    return (0);
}
