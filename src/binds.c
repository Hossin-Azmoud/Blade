#include <mi.h>

void editor_handle_binding(Lines_renderer *line_ren, vKeyBindingQueue *bindings)
{
    editor_identify_binding(bindings);
   switch (bindings->kind)
    {
        case COPY_LINE: {
            CLIPBOARD_SET(line_ren->current->content);
        } break;
        case INDENT_LINE: {
            int x = line_ren->current->x;
            line_ren->current->x = 0;
            editor_tabs(line_ren->current);
            line_ren->current->x += x;
        } break;
        case DEL_LINE: {
            CLIPBOARD_SET(line_ren->current->content);
            if (line_ren->current->prev != NULL) {
                line_ren->current->size = 0; // Set the size to zero so the line can be disconnected and freed!
                line_disconnect_from_ren(line_ren);
                return;
            }
            
            line_ren->current->x = 0;
            memset(line_ren->current->content, 0, line_ren->current->size);
            line_ren->current->size = 0; // Set the size to zero so the line can be disconnected and freed!
        } break;
        case NOT_VALID: {
        } break;
        default: {} break;
    }
}


void editor_identify_binding(vKeyBindingQueue *bindings)
{
    if (!bindings->size)
    {
        bindings->kind = NOT_VALID;
        return;
    }

    if (bindings->keys[0] == 'y' && bindings->keys[1] == 'y') { // Copy the current line into clipboard.
        bindings->kind = COPY_LINE;
        return;
    }

    if (bindings->keys[0] == 'd' && bindings->keys[1] == 'd') { // delete the current line into clipboard.
        bindings->kind = DEL_LINE;
        return;
    }

    if (bindings->keys[0] == '>' && bindings->keys[1] == '>') { // delete the current line into clipboard.
        bindings->kind = INDENT_LINE;
        return;
    }

}
