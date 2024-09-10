#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  char *s = NULL;
  if (argc < 2) {
    fprintf(stderr, "[ERROR] expected a string to split.");
    return 1;
  }
  s = *(argv + 1);
  for (char *next = strtok(s, "T ");next;(next = strtok(NULL, "T "))) {
    printf("; %s\n", next);
  }
  return 0;
}
