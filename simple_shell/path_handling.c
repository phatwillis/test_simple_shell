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

int is_command_in_path(char *command, char *path_dirs[]) {
    while (*path_dirs != NULL) {
        char command_path[MAX_PATH_SIZE];
        snprintf(command_path, sizeof(command_path), "%s/%s", *path_dirs, command);
        
        if (access(command_path, X_OK) == 0) {
            return 1; // Command found in the current directory
        }
        
        path_dirs++;
    }

    return (0); // Command not found in any directory in PATH
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *path = getenv("PATH");

    if (path == NULL) {
        write(STDERR_FILENO, "Error: PATH environment variable not set\n", 40);
        return EXIT_FAILURE;
    }

    // Tokenize the PATH variable into individual directories
    char *path_dirs[MAX_PATH_SIZE];
    char *path_token = strtok(path, ":");

    int i = 0;
    while (path_token != NULL && i < MAX_PATH_SIZE - 1) {
        path_dirs[i++] = path_token;
        path_token = strtok(NULL, ":");
    }
    path_dirs[i] = NULL; // Set the last element to NULL

    for (write(STDOUT_FILENO, ":) ", 3); fgets(input, sizeof(input), stdin)
     != NULL; write(STDOUT_FILENO, ":) ", 3)) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Tokenize the input into command and arguments
        char *token = strtok(input, " ");
        char *args[MAX_INPUT_SIZE];
        int j = 0;

        while (token != NULL && j < MAX_INPUT_SIZE - 1) {
            args[j++] = token;
            token = strtok(NULL, " ");
        }
        args[j] = NULL; // Set the last element to NULL for execve

        // Check if the command exists in any directory listed in PATH
        if (is_command_in_path(args[0], path_dirs)) {
            // Execute the command with arguments
            execute_command(args);
        } else {
            write(STDERR_FILENO, "Error: Command not found\n", 25);
        }
    }

    return (0);
}
