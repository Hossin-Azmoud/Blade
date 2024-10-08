// #include "parser.h"
// #include <cstdio>
#include <fcntl.h>
#include <linux/limits.h>
#include <blade.h>
#include <stdio.h>
#define INIT_SCRIPT_PATH "init.mi"
static char *temp = NULL;
// TODO: Implement -v/--version flags and --help
void editor_help(char *program) {
  printf("\nusage => %s <file/directory>\n", program);
  printf("flags:\n");
  printf("-h, --help    => Prints the help message into stdout.\n");
  printf("-v, --version => Prints the version of the editor.\n\n");
}

bool check_args(int argc, char **argv) {
  char *program = *argv;
  bool flag = true;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
      printf(MI_V);
      flag = false;
    }
    if (!strcmp(argv[i], "-h") || !strcmp(argv[1], "--help")) {
      editor_help(program);
      flag = false;

    }
    if (!strcmp(argv[i], "-c") || !strcmp(argv[1], "--cfg")) {
      if (i + 1 < argc) {
        temp = argv[i + 1];
        flag = true;
      } else {
        fprintf(stderr, "-c || --cfg was specified but no file path was specified.");
        flag = false;
      }
    }
  }

  return flag;
}

// #define EXP
int main(int argc, char **argv) {
  (void)argc;
  int ret = 0;
#ifdef EXP
  // (void)argv;
  // (void)argc;
  if (argc == 3) {
    int cmp = xstr(argv[1], argv[2]);
    char *cmp_s = (cmp == 0) ? "Exists inside" : "does not exist inside";
    printf("%s %s inside %s\n", argv[1], cmp_s, argv[2]);
  }
#else
  if (check_args(argc, argv)) {
    ret = editor(argv, temp);
    CLIPBOARD_FREE();
    // THIS IS BECAUSE THE EDITOR SCREWS UP THE TERMINAL.
    system("reset");
  }
#endif /* ifdef EXP */
  return ret;
}
