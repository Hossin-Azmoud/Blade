#include <mi.h>

void editor_handle_binding(Lines_renderer *line_ren, vKeyBindingQueue *bindings, FileBrowser *fb)
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
        
        case UNINDENT_LINE: {
            int x = line_ren->current->x;
            
            line_ren->current->x = 0;
            while (line_ren->current->content[line_ren->current->x] == ' ' && line_ren->current->x < 4) {
                line_ren->current->x++;
            }

            memmove(line_ren->current->content, 
                    line_ren->current->content + line_ren->current->x, 
                    line_ren->current->size - line_ren->current->x);

            line_ren->current->size -= line_ren->current->x;
            line_ren->current->x = (x - line_ren->current->x);
        } break;

        case DEL_LINE: {
            CLIPBOARD_SET(line_ren->current->content);
            if (line_ren->current->prev != NULL) {
                line_ren->current = disconnect_line(line_ren->current);
    
                if (line_ren->current->next != NULL) {
                    line_ren->current = line_ren->current->next;
                    if (line_ren->end->next != NULL) {
                        line_ren->end = line_ren->end->next;
                    }
                }
                
                line_ren->count--;
                return;
            }
            
            line_ren->current->x = 0;
            memset(line_ren->current->content, 0, line_ren->current->size);
            line_ren->current->size = 0; // Set the size to zero so the line can be disconnected and freed!
        } break;
        case DIR_MODE: {
            // TODO: save the file then change to [..] dir 
            save_file(fb->open_entry_path, line_ren->origin, false);
            fb   = realloc_fb(fb, "..");

            // if (entry.type == FILE__) {
            //     E->renderer->file_type = get_file_type (E->fb->open_entry_path);
            //     load_file(E->fb->open_entry_path, E->renderer);
            // }
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
    
    if (bindings->keys[0] == '<' && bindings->keys[1] == '<') { // delete the current line into clipboard.
        bindings->kind = UNINDENT_LINE;
        return;
    }
    
    if (bindings->keys[0] == '.' && bindings->keys[1] == '.') { // delete the current line into clipboard.
        bindings->kind = DIR_MODE;
        return;
    }
    
    bindings->kind = NOT_VALID;
    return;
}
