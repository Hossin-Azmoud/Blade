#include "parser.h"
#include <stdio.h>
/*** 
  Docs:
  Defauls
    autosave: false,
    indent_char: " ",
    indent_count: 4
***/


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
  printf("}\n");
  
  free(cfg);
  return (0);
}

