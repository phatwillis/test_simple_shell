#include "shell.h"

char *custom_get_line(void) {
    static char buffer[MAX_INPUT_SIZE];
    static int buffer_index = 0;
    static ssize_t bytes_read = 0;

    if (buffer_index >= bytes_read || bytes_read == 0) {
        // Read more characters into the buffer
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Reset buffer_index
        buffer_index = 0;

        // Check for end-of-file
        if (bytes_read == 0) {
            return NULL;
        }
    }

    // Get the next line from the buffer
    char *line = &buffer[buffer_index];
    char *newline = strchr(line, '\n');

    if (newline != NULL) {
        // Replace newline with null terminator
        *newline = '\0';
        // Move buffer_index to the next character after the newline
        buffer_index = (int)(newline - buffer) + 1;
    } else {
        // No newline found, set buffer_index to the end
        buffer_index = bytes_read;
    }

    return (line);
}

int main() {
    char *input;

    do {
        write(STDOUT_FILENO, "$ ", 2);
        input = custom_get_line();

        if (input != NULL) {
            write(STDOUT_FILENO, "You entered: ", 13);
            write(STDOUT_FILENO, input, strlen(input));
            write(STDOUT_FILENO, "\n", 1);
        }
    } while (input != NULL);

    return (0);
}
