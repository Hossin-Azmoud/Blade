#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _GNU_SOURCE
int main(void) 
{
  char *command = NULL;
  char  *output = NULL;
  ssize_t n, nread, onread;
  FILE *f;
  do {
    if (command) {
      free(command);
      command = NULL;
    }
    printf("> ");
    nread = getline(&command, 
    &n, stdin);
    if (nread == -1)
      break;
    f = popen(command, "r");
    do {
      onread = getline(&output, &n, f);
      if (onread > 0) {
        printf("%s", output);
        if (output)
          free(output);
        output = NULL;
      }
    } while (onread > 0);
    pclose(f);
  } while (nread != (ssize_t)-1 && strcmp(command, "quit"));
  return (0);
}
