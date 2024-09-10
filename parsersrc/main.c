#include "parser.h"
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
  free(cfg);
    return (0);
}
