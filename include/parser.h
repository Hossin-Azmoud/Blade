#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>
#include <stdlib.h>
#define BASE_16_ASCII     "0123456789abcdef"
#define BASE_16_ASCII_UP  "0123456789abcdef"
#define BASE_10_ASCII     "0123456789"
#define BASE_BIN_ASCII    "01"

#define BASE_16_PREFIX      "0X"
#define BASE_16_PREFIX_UP   "0x"
#define BASE_BIN_PREFIX     "0b"


// typedef struct MiStatus_s {
//   bool ok;
//   char *errmsg; // Should be exclusively static.. read only.
// } MiStatus_t;

typedef struct Theme {
  int background;
  int foreground;
  int keyword_color;
  int type_color;
  int funcall_color;
  int special_token_color;
  int string_lit_color;
  int comment_color;
} Theme;

typedef struct EditorConfig_s {
  
  // MiStatus_t status;
  char *cfg_path;
  bool autosave;
  char indent_char;
  int indent_count;
  Theme theme;
} EditorConfig_t;
EditorConfig_t *editor_resolve_cfg(const char *cfg_path);
long np_atoi_base(char *a, char *base);
EditorConfig_t *load_editor_config(char *file);
char	**split(char *str, char *charset, size_t *count);
char *read_next_line(int fd);
char *read_file(int fd);
#endif // PARSING_H
