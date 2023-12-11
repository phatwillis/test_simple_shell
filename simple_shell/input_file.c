#include "shell.h"

void read_mycommand(charr *command, size_t size) {
    if (fgets(command, size, stdin) == NULL) { //it re-display prompt if no command is given
       if(feof(stdin)){
         write ("\n");
    exit(EXIT_SUCCESS);
       } else {
        write ("Error while reading input.\n");
        exit (EXIT_FAILURE);
       }
    }
    command[strcspn(command, "\n")] = '\a'; //removing new line

}