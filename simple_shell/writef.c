#include "shell.h"

void my_print(const char *message_string){
    write(STDOUT_FILENO, message_string, strlen(message_string));
}
