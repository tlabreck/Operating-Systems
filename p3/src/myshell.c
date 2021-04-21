#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

#define MAX_LINE 4095
#define LINE_BUFFER MAX_LINE + 3

typedef struct {
  int argc;
  char **argv;
  char *str; // to store the command line in the implementation of the hist command
} command_t;

typedef struct {
  char *name;
  int (*function)(int argc,char *argv[]);
} cmd_entry;

int cmd_author(int argc, char *argv[]) {
  printf("Name: Jorge Fandinno\n");
  return 0;
}

int cmd_exit(int argc, char *argv[]) {
  exit(0);
}

int cmd_cdir(int argc, char *argv[]) {
  char path[MAXPATHLEN];
  if (argc==1) {
    if (getcwd(path, MAXPATHLEN) == NULL) perror("cdir");
    else printf("%s\n",path);
    return 0;
  }
  if (chdir(argv[1])==-1) perror("cdir");
  return 0;
}

cmd_entry cmd_table[] = {
  {"author",cmd_author},
  {"exit",cmd_exit},
  {"cdir",cmd_cdir},
  {NULL,NULL}
};

int parse_args(char *line, char ***p_argv) {
  int argc=0;
  char **argv = *p_argv;
  for (argc=0, argv[argc]=strtok(line," \t\n"); argv[argc]!=NULL && argc < _POSIX_ARG_MAX; argv[++argc]=strtok(NULL," \t\n"));
  return argc;
}

int get_command(command_t *cmd, int echo){
  static char line_buffer[LINE_BUFFER];
  clearerr(stdin);
  if (fgets(line_buffer, LINE_BUFFER, stdin) == NULL){
    if(ferror(stdin))perror("myshell");
    else exit(0);
  }
  if (strnlen(line_buffer, LINE_BUFFER) > MAX_LINE + 1){ // MAX_LINE + 1 accounts for the end line character
    fprintf(stderr, "command too long\n");
    cmd->argc = -1;
    return cmd->argc;
  }
  if(echo) printf("%s",line_buffer);
  cmd->argc = parse_args(line_buffer, &cmd->argv);
  return cmd->argc;
}

void process_command(command_t *command){
  int i;
  if(command->argc <= 0)
    return;
  for (i=0; ; i++) {
    if (cmd_table[i].name == NULL) {
      fprintf(stderr, "Unrecognized command: %s\n", command->argv[0]);
      return;
    }
    if (!strcmp(command->argv[0], cmd_table[i].name)) {
      cmd_table[i].function(command->argc, command->argv);
      return;
    }
  }
}

int main(int argc, char *argv[]) {
  int echo = argc > 1 && !strcmp(argv[1], "--echo");
  static char *arg_buffer[_POSIX_ARG_MAX];
  command_t command;
  command.argv = arg_buffer;
  while (1) {
    printf("@> ");
    get_command(&command, echo);
    process_command(&command);
  }
}