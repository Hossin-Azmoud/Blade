#include "parser.h"
#include <stdio.h>


int main(int count, char **vec) {
  char *file;
  if (count < 2) {
      file = "config.mi";
  } else {
      file = vec[1];
  }
  // TODO: Make the load_editor_config() func.
  EditorConfig_t *cfg = load_editor_config(file);
  // TODO: integrate the config into the editor.
  // > Come up with new config fields.
  printf("-------Parsed configuration------\n");
  printf("{\n");
    printf("  autosave: %s, \n", cfg->autosave ? "true" : "false");
    printf("  indent_char: %c, \n", cfg->indent_char);
    printf("  indent_count: %d, \n", cfg->indent_count);
    printf("  back: %x, \n", cfg->theme.background);
    printf("  front: %x, \n", cfg->theme.foreground);
    printf("  keyword_color: %x, \n", cfg->theme.keyword_color);
    printf("  type_color: %x, \n", cfg->theme.type_color);
    printf("  funcall_color: %x, \n", cfg->theme.funcall_color);
    printf("  special_token_color: %x, \n", cfg->theme.special_token_color);
    printf("  string_lit_color: %x, \n", cfg->theme.string_lit_color);
    printf("  comment_color: %x, \n", cfg->theme.comment_color);
  printf("}\n");
  
  free(cfg);
  return (0);
}

