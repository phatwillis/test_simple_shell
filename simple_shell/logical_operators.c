#include "shell.h"
void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid == 0) {
        /* Child process */
        if (execvp(args[0], args) == -1) {
            perror("Error");
            _exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        /* Forking failed */
        perror("Error");
    } else {
        /* Parent process */
        /* Wait for the child process to finish */
        wait(NULL);
    }
}

/* Split the input into command and arguments */
int tokenize_input(char *input, char *args[]) {
    int i = 0;
    int arg_start = 0;
    int in_arg = 0;

    while (input[i] != '\0') {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            if (in_arg) {
                /* End of argument */
                args[arg_start] = &input[arg_start];
                input[i] = '\0'; /* Null-terminate the argument */
                in_arg = 0;
            }
        } else {
            if (!in_arg) {
                /* Start of a new argument */
                arg_start = i;
                in_arg = 1;
            }
        }

        i++;
    }

    if (in_arg) {
        /* Handle the last argument */
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

int is_command_cd(char *command) {
    return (strcmp(command, "cd") == 0);
}

void print_error(char *message) {
    write(STDERR_FILENO, message, (strlen(message)));
}

int main() {
    char input[MAX_INPUT_SIZE];

    for (write(STDOUT_FILENO, "$ ", 2); read(STDIN_FILENO, input,
    sizeof(input)) > 0; write(STDOUT_FILENO, "$ ", 2)) {
        /* Remove the newline character from the input */
        input[strcspn(input, "\n")] = 0;

        /* Tokenize the input into command and arguments */
        char *commands[MAX_INPUT_SIZE];
        char *args[MAX_INPUT_SIZE];

        int i = 0;
        int command_start = 0;

        while (input[i] != '\0') {
            if (input[i] == ';' || input[i] == '&' || input[i] == '|') {
                /* End of command */
                input[i] = '\0'; /* Null-terminate the command */
                commands[command_start] = &input[command_start];

                /* Tokenize the command into arguments */
                int arg_count = tokenize_input(commands[command_start], args);
                
                /* Execute the command with arguments */
                execute_command(args);

                /* Reset for the next command */
                command_start = i + 1;

                /* Check for logical operators */
                if (input[i] == '&' && input[i + 1] == '&') {
                    /* Execute the next command only if the previous one succeeded */
                    if (WEXITSTATUS(status) == 0) {
                        i++;
                    }
                } else if (input[i] == '|' && input[i + 1] == '|') {
                    /* Execute the next command only if the previous one failed */
                    if (WEXITSTATUS(status) != 0) {
                        i++;
                    }
                }
            }

            i++;
        }

        /* Execute the last command */
        if (command_start < i) {
            input[i] = '\0'; /* Null-terminate the last command */
            commands[command_start] = &input[command_start];

            /* Tokenize the command into arguments */
            int arg_count = tokenize_input(commands[command_start], args);
            
            /* Execute the command with arguments */
            execute_command(args);
        }
    }

    return (0);
}
