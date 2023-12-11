#include "shell.h"

int main(void){
    char command[115];

  do {
     looping_display_prompt();
        read_mycommand(command, sizeof(command));
        execute_command(command);
} while (1);
 
    return (0);
}