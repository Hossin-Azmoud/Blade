#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*** 
    Docs:
    Defauls
        autosave: false,
        indent_char: " ",
        indent_count: 4
***/

typedef struct EditorConfig_s {
    bool autosave; // Unused
    char indent_char;
    int indent_count;
} EditorConfig_t;

int main(int count, char **vec) {
    char *file;
    if (count < 2) {
        file = "miconfig.mi";
    } else {
        file = argv[1];
    }
    // TODO: Make the load_editor_config() func.
    EditorConfig_t *cfg = load_editor_config(file);
    // TODO: integrate the config into the editor.
    // > Come up with new config fields.
    return (0);
}
