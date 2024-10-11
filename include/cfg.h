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

typedef enum cfg_action_e {
  CFG_SET,
  CFG_GET
} cfg_action_t;


typedef struct Theme {
  int background, 
  foreground,
  keyword_color,
  type_color, 
  funcall_color,
  special_token_color, 
  string_lit_color, 
  comment_color, 
  highlight_bg_fb, highlight_fg_fb,
  highlight_bg_vmode, highlight_fg_vmode,
  details_bar_bg, details_bar_fg,
  command_prompt_bg, command_prompt_fg,
  blade_mode_bg, blade_mode_fg;
} Theme;



typedef struct EditorConfig_s {
  
  // MiStatus_t status;
  char *cfg_path;
  bool autosave;
  char indent_char;
  int indent_count;
  Theme theme;
} EditorConfig_t;

bool write_default_cfg(char *xdg_cfg_path);
EditorConfig_t *cfg_interface(cfg_action_t a, EditorConfig_t *data);
EditorConfig_t *editor_resolve_cfg(const char *cfg_path);
EditorConfig_t *alloc_default_cfg(void);
long np_atoi_base(char *a, char *base);
EditorConfig_t *load_editor_config(char *file);
char	**split(char *str, char *charset, size_t *count);
char *read_next_line(int fd);
char *read_file(int fd);
void release_cfg(EditorConfig_t *cfg);
#endif // PARSING_H
