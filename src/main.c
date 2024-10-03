// #include "parser.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <blade.h>
#include <stdio.h>
#define INIT_SCRIPT_PATH "init.mi"

// TODO: Implement -v/--version flags and --help
void editor_help(char *program) {
  printf("\nusage => %s <file/directory>\n", program);
  printf("flags:\n");
  printf("-h, --help    => Prints the help message into stdout.\n");
  printf("-v, --version => Prints the version of the editor.\n\n");
}

bool check_args(int argc, char **argv) {
  char *program = *argv;
  if (argc >= 2) {
    if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
      printf(MI_V);
      return true;
    }
    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      editor_help(program);
      return true;
    }
  }

  return false;
}

/***
 * Look for the config file in two places.
 * 1 - ~/.config/<name>/cfg.mi
 * 2 - args --config <path> or -c <config>
 * if it does not exist make it then write a basic default config.
 * else just load it and use it throught the editor
 *
 * **/
// #define LEN(A) (sizeof(A)/sizeof(A[0]))

// EditorConfig_t *resolve_cfg(const char *path) {
//   char *Home = "HOME";
//   char *value = getenv(Home);
//   char xdg_cfg_path[PATH_MAX];
//   sprintf(xdg_cfg_path, "%s/.config/micfg", value);
//   // switch x = (get_entry_type("~/.bashrc"));

//   switch (get_entry_type("~/.bashrc")) {
//     case FILE__: {
//       int fd = open("~/.bashrc", O_WRONLY);
//       char *s = read_file(fd);
//       printf("%s", s);
//     } break;
//     case DIR__: {
//       printf("Can not read dir.\n");
//     } break;
//     default:
//     case NOT_EXIST: {
//       printf("~/.bashrc does not exit tho.;\n");
//     } break;
//   }
// }

// #define EXP
int main(int argc, char **argv) {
  (void)argc;
  int ret = 0;

#ifdef EXP
  (void)argv;
  (void)argc;


#else
  if (!check_args(argc, argv)) {
    ret = editor(argv);
    CLIPBOARD_FREE();
  }
#endif /* ifdef EXP */
  return ret;
}
