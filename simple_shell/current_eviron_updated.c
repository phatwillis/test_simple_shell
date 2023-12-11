#include "shell.h"


void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
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

// Split the input into command and arguments
int tokenize_input(char *input, char *args[]) {
    int i = 0;
    int arg_start = 0;
    int in_arg = 0;

    while (input[i] != '\0') {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            if (in_arg) {
                // End of argument
                args[arg_start] = &input[arg_start];
                input[i] = '\0'; // Null-terminate the argument
                in_arg = 0;
            }
        } else {
            if (!in_arg) {
                // Start of a new argument
                arg_start = i;
                in_arg = 1;
            }
        }

        i++;
    }

    if (in_arg) {
        // Handle the last argument
        args[arg_start] = &input[arg_start];
    }

    return (i);
}

int is_command_exit(char *command) {
    return (strcmp(command, "exit") == 0);
}

int is_command_setenv(char *command) {
    return (strcmp(command, "setenv") == 0);
}

int is_command_unsetenv(char *command) {
    return (strcmp(command, "unsetenv") == 0);
}

void print_error(char *message) {
    write(STDERR_FILENO, message, (strlen(message)));
}

int main() {
    char input[MAX_INPUT_SIZE];

    for (write(STDOUT_FILENO, "$ ", 2); read(STDIN_FILENO, input,
     sizeof(input)) > 0; write(STDOUT_FILENO, "$ ", 2)) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Tokenize the input into command and arguments
        char *args[MAX_INPUT_SIZE];
        int i = tokenize_input(input, args);

        // Check if the command is the exit built-in
        if (is_command_exit(args[0])) {
            if (args[1] != NULL) {
                int exit_status = atoi(args[1]);
                _exit(exit_status);
            } else {
                _exit(EXIT_SUCCESS);
            }
        }

        // Check if the command is the setenv built-in
        if (is_command_setenv(args[0])) {
            if (args[1] != NULL && args[2] != NULL) {
                if (setenv(args[1], args[2], 1) != 0) {
                    write(STDERR_FILENO, "Error: setenv failed\n", (21));
                }
            } else {
                write(STDERR_FILENO, "Usage: setenv VARIABLE VALUE\n", (29));
            }
        }

        // Check if the command is the unsetenv built-in
        if (is_command_unsetenv(args[0])) {
            if (args[1] != NULL) {
                if (unsetenv(args[1]) != 0) {
                    write(STDERR_FILENO, "Error: unsetenv failed\n", (23));
                }
            } else {
                write(STDERR_FILENO, "Usage: unsetenv VARIABLE\n", (25));
            }
        }

        // Check if the command exists in any directory listed in PATH
        if (access(args[0], X_OK) == 0) {
            // Execute the command with arguments
            execute_command(args);
        } else {
            write(STDERR_FILENO, "Error: Command not found\n", (25));
        }
    }

    return (0);
}
