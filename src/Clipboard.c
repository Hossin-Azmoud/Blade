#include <mi.h>

// Abstract part of the clip api.
void *clipboard(ClipBoardEvent e, char *data)
{
    static Chunk *clipboard = NULL;

    switch (e) {
        case CFREE: { 
            chunk_distroy(clipboard);
        } break;
        case CGET: {
            return (void *)(clipboard);
        } break;
        case CSET: {
            if (clipboard == NULL) {
                clipboard = chunk_new();
            }

            memset(clipboard->data, 0, clipboard->cap);
            clipboard->size = 0;
            chunk_append_s(clipboard, data);
        } break;
        default: {}; // unreachable code. NORMALLY haha.
    }

    return NULL;
}


void editor_push_data_from_clip(Lines_renderer *line_ren)
{
    Chunk *chunk = CLIPBOARD_GET();
    // Line  *temp  = line_ren->current;
    char *data   = chunk->data; 

    if (data) {
        // line_ren->current->x = line_ren->current->size; 
        for (; *data; data++) {
            if ((*data == '\n') && (chunk->lines) >= 1) {
                
                if (!(line_ren->current->next) || line_ren->current->size > 0) {
                    line_ren->current->x = line_ren->current->size;
                    editor_new_line(line_ren, true);
                    continue;
                }
            }

            line_push_char(line_ren->current, *data, true);
        }
    }

    // line_ren->current    = temp;
}

