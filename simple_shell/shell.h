#include SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>


#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024
#define MAX_ALIAS_SIZE 64
#define MAX_ALIAS_COUNT 10

void looping_display_prompt(void);
void myprint(const char *message);
void read_mycommand(char *command, size_t size);
void execute_command(const char *command);
void execute_command(char *args[]);
void print_environment();
void print_error(char *message);
void print_aliases();
void print_alias(char *alias_name);
void define_alias(char *alias_name, char *alias_value);
void replace_variables(char *arg);

#endif /*SHELL_H*/