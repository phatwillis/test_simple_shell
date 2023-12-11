#include "shell.h"

int status; // Define status variable for child process exit status

typedef struct {
    char name[MAX_ALIAS_SIZE];
    char value[MAX_INPUT_SIZE];
} Alias;

Alias aliases[MAX_ALIAS_COUNT];
int alias_count = 0;

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
        // Wait for the child process to finish and get the status
        wait(&status);
    }
}

// Replace variables in a string
void replace_variables(char *arg) {
    char *variable = arg + 1; // Skip the '$' character

    if (strcmp(variable, "?") == 0) {
        // Replace with exit status
        sprintf(arg, "%d", WEXITSTATUS(status));
    } else if (strcmp(variable, "$") == 0) {
        // Replace with process ID
        sprintf(arg, "%d", getpid());
    } else if (strcmp(variable, "PATH") == 0) {
        // Replace with the actual value of the PATH variable
        // You may need to implement PATH handling based on your requirements
        // For now, let's assume the value is "/your/path"
        strcpy(arg, "/your/path");
    }
    // Add more variable handling as needed
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

                // Check for variable replacement
                if (args[arg_start][0] == '$') {
                    replace_variables(args[arg_start]);
                }

                in_arg = 0;
            }
        } else if (input[i] == '#') {
            // Ignore characters after '#' until the end of the line
            break;
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

        // Check for variable replacement
        if (args[arg_start][0] == '$') {
            replace_variables(args[arg_start]);
        }
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

int is_command_cd(char *command) {
    return (strcmp(command, "cd") == 0);
}

int is_command_alias(char *command) {
    return (strcmp(command, "alias") == 0);
}

void print_error(char *message) {
    write(STDERR_FILENO, message, strlen(message));
}

void print_aliases() {
    for (int i = 0; i < alias_count; i++) {
        write(STDOUT_FILENO, "alias %s='%s'\n", aliases[i].name, aliases[i].value);
    }
}

void print_alias(char *alias_name) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, alias_name) == 0) {
            write(STDOUT_FILENO, "alias %s='%s'\n", aliases[i].name, aliases[i].value);
            return;
        }
    }

    write(STDERR_FILENO, "alias: %s not found\n", alias_name);
}

void define_alias(char *alias_name, char *alias_value) {
    if (alias_count >= MAX_ALIAS_COUNT) {
        write(STDERR_FILENO, "alias: Maximum number of aliases reached\n");
        return;
    }

    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, alias_name) == 0) {
            // Alias already exists, replace its value
            strcpy(aliases[i].value, alias_value);
            return;
        }
    }

    // Alias doesn't exist, create a new one
    strcpy(aliases[alias_count].name, alias_name);
    strcpy(aliases[alias_count].value, alias_value);
    alias_count++;
}

int main() {
    char input[MAX_INPUT_SIZE];

    for (write(STDOUT_FILENO, "$ ", 2); read(STDIN_FILENO, input,
     sizeof(input)) > 0; write(STDOUT_FILENO, "$ ", 2)) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        char *commands[MAX_INPUT_SIZE];
        char *args[MAX_INPUT_SIZE];

        int i = 0;
        int command_start = 0;

        while (input[i] != '\0') {
            if (input[i] == ';' || input[i] == '&' || input[i] == '|') {
                // End of command
                input[i] = '\0'; // Null-terminate the command
                commands[command_start] = &input[command_start];

                // Tokenize the command into arguments
                int arg_count = tokenize_input(commands[command_start], args);

                // Handle alias command
                if (is_command_alias(args[0])) {
                    if (arg_count == 1) {
                        // Print all aliases
                        print_aliases();
                    } else {
                        // Print specified aliases or define new ones
                        for (int j = 1; j < arg_count; j++) {
                            char *arg = args[j];

                            if (strchr(arg, '=')) {
                                // Define alias
                                char *alias_name = strtok(arg, "=");
                                char *alias_value = strtok(NULL, "=");

                                define_alias(alias_name, alias_value);
                            } else {
                                // Print alias
                                print_alias(arg);
                            }
                        }
                    }
                } else {
                    // Execute the command
                    execute_command(args);
                }

                command_start = i + 1;

                // Check for logical operators
                if (input[i] == '&' && input[i + 1] == '&') {
                    // Execute the next command only if the previous one succeeded
                    if (WEXITSTATUS(status) == 0) {
                        i++;
                    }
                } else if (input[i] == '|' && input[i + 1] == '|') {
                    // Execute the next command only if the previous one failed
                    if (WEXITSTATUS(status) != 0) {
                        i++;
                    }
                }
            }

            i++;
        }

        // Execute the last command
        if (command_start < i) {
            input[i] = '\0'; // Null-terminate the last command
            commands[command_start] = &input[command_start];

            // Tokenize the command into arguments
            int arg_count = tokenize_input(commands[command_start], args);

            // Handle alias command
            if (is_command_alias(args[0])) {
                if (arg_count == 1) {
                    // Print all aliases
                    print_aliases();
                } else {
                    // Print specified aliases or define new ones
                    for (int j = 1; j < arg_count; j++) {
                        char *arg = args[j];

                        if (strchr(arg, '=')) {
                            // Define alias
                            char *alias_name = strtok(arg, "=");
                            char *alias_value = strtok(NULL, "=");

                            define_alias(alias_name, alias_value);
                        } else {
                            // Print alias
                            print_alias(arg);
                        }
                    }
                }
            } else {
                // Execute the command
                execute_command(args);
            }
        }
    }

    return (0);
}
