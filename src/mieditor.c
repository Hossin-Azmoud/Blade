#include <mi.h>

static Vec2 vec2() {
    return (Vec2) {.x = 0, .y=0, ._line=NULL};
}

MiEditor *init_editor(const char *path)
{
    MiEditor *E = malloc(sizeof(MiEditor));

    E->ewindow = init_ncurses_window();
    E->renderer= malloc(sizeof(Lines_renderer));
    editor_load_layout(E);

    // If the caller did not supply a file then we ask him in a seperate screen.
    if (path == NULL) {
        E->file = editor_render_startup(E->renderer->win_w / 2, E->renderer->win_h / 2);
    } else {
        // Load the file that was given to the editor from argv.
        int sz = strlen(path);
        E->file = malloc(sz + 1);
        E->file = memcpy(E->file, path, sz);
    }

    E->fb = new_file_browser(E->file);

    // Prepare for highlighting text (Copying and pasting..)
    E->highlighted_end         = vec2() ; // x=0, Y=0
    E->highlighted_start       = vec2() ; // x=0, Y=0
    E->highlighted_data_length = 0;

    // Prepare notification_buffer and other important properties..
    E->notification_buffer = malloc((1024 + 1) * sizeof(char));
    memset(E->notification_buffer, 0, 1024);
    E->exit_pressed = false;
    E->char_deleted = false;
    E->mode         = NORMAL;
    
    // Prepare the renderer.
    E->renderer->origin      = Alloc_line_node(0);
    E->renderer->start       = E->renderer->origin;
    E->renderer->end         = E->renderer->origin; 
    E->renderer->current     = E->renderer->origin; 
    E->renderer->max_padding = 2;

    // If the path that was passed was a file, or if it does not exist. we assign.
    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
        E->renderer->script_type = get_script_type (E->file);
        load_file(E->file, E->renderer);
    }
    // Init binding queue.    
    E->binding_queue = (vKeyBindingQueue){0x0};
    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
        editor_refresh(E);
    } else {
        render_file_browser(E);
    }
    
    return (E);
}



void editor_load_layout(MiEditor *E)
{
    // Set the dimentions of the edittor..
    getmaxyx(E->ewindow, 
         E->renderer->win_h, 
         E->renderer->win_w
    );
}

void release_editor(MiEditor *E)
{
    if (!E) return;
    release_fb(E->fb);
    free_lines(E->renderer->origin);
    free(E->renderer);
    free(E->file);
    free(E->notification_buffer);
    free(E);
}

void editor_refresh(MiEditor *E)
{
    erase();
    render_lines(E->renderer);
    editor_details(E->renderer, E->file, E->mode, E->notification_buffer);
    editor_apply_move(E->renderer);
    refresh();
}

void editor_render(MiEditor *E)
{
    // Check exit signal 
    if (E->exit_pressed) {
        return;
    }
    
    // Check if a key was delted.
    if (E->char_deleted) {
        delch();
        E->char_deleted = false;
    }
 
    erase();
    if (E->fb->type == DIR__) {
        render_file_browser(E);
        return;
    }
    
    // Render Lines.     
    curs_set(1);
    render_lines(E->renderer);
    if (E->mode == VISUAL) {
        E->highlighted_data_length = highlight_until_current_col(E->highlighted_start, E->renderer);
        sprintf(E->notification_buffer, "[ %d bytes were Highlighted]\n", E->highlighted_data_length);
    } else if (E->mode == NORMAL) {
        sprintf(E->notification_buffer, "[BINDING: %s]\n", E->binding_queue.keys);
        if (E->binding_queue.size == MAX_KEY_BINDIND) {
            // TODO: Process Key E->binding_queue.
            editor_handle_binding(E->renderer, &E->binding_queue);
            memset(E->binding_queue.keys, 0, E->binding_queue.size);
            E->binding_queue.size = 0;
        }
    }

    editor_details(E->renderer, E->file, E->mode, E->notification_buffer);
    if (strlen(E->notification_buffer) > 0) memset(E->notification_buffer, 0, 1024);
    editor_apply_move(E->renderer);
    refresh();
}

void render_file_browser(MiEditor *E)
{    
    erase();
    curs_set(0);
    size_t padding = 5;
    size_t row = E->fb->cur_row;
    
    if (E->fb->type == DIR__) {
        for (size_t y = 0; y < E->fb->size; y++) {
            BrowseEntry entry = E->fb->entries[y];
            mvprintw(y + padding, padding, entry.value);
            if (row == y) {
                colorize(row + padding, padding, 
                    strlen(entry.value), 
                    HIGHLIGHT_WHITE);
            }
        }
    }
}

void editor_update(int c, MiEditor *E)
{
    // Globals.
    switch (c) {
        case KEY_F(2): {
            E->exit_pressed = true;
            return;
        } break;
        case ESC: {
            E->mode = NORMAL;
            return;
        } break;
    }

    // Actions that depend on the mode.
    switch (E->mode) {
        case VISUAL: {
            if (c == KEY_COPY_) {
                // Store into about end of the highlighting in some struct.
                E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
                E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
                E->highlighted_end._line =  E->renderer->current;
                E->mode = NORMAL;
                clipboard_save_chunk(E->highlighted_start, E->highlighted_end, false);
            }
            if (c == KEY_CUT_) {
                // Store into about end of the highlighting in some struct.
                E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
                E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
                E->highlighted_end._line =  E->renderer->current;
                E->mode = NORMAL;
                clipboard_save_chunk(E->highlighted_start, E->highlighted_end, true);
                
            }
        } break;
        
        case NORMAL: {
            switch (c) {
                case KEY_PASTE_: {
                    // TODO: Make clipboard be synced with the VISUAL mode clipboard_
                    editor_push_data_from_clip(E->renderer);
                } break;
                case KEY_INSERT_: {
                    E->mode = INSERT;
                } break;
                case KEY_VISUAL_: {
                    // we mark the chords of the start position!
                    E->mode = VISUAL;
                    E->highlighted_start.x = E->renderer->current->x;
                    E->highlighted_start.y = E->renderer->current->y;
                    E->highlighted_start._line = E->renderer->current;
                } break;
                case KEY_SAVE_: {
                    int saved_bytes = save_file(E->file, E->renderer->origin, false);
                    sprintf(E->notification_buffer, "[ %dL %d bytes were written ]\n", 
                        E->renderer->count, 
                        saved_bytes);
                } break; 
                default: {
                    if (E->binding_queue.size < MAX_KEY_BINDIND) {
                        E->binding_queue.keys[E->binding_queue.size++] = (char) c;
                        // TODO: Make binding copy whole line with `yy`, and delete whole line with `dd`
                    }
                } break;
            }
        } break;
        case INSERT: {
            switch (c) {
                case TAB: {
                    editor_tabs(E->renderer->current);
                } break;
                case KEY_BACKSPACE: { 
                    editor_backspace(E->renderer);
                    E->char_deleted = true;
                } break;
                case KEY_DEL: {
                    // TDO: Implement this.
                    editor_dl(E->renderer->current);
                    E->char_deleted = true;
                } break;		
                case NL: {
                    editor_new_line(E->renderer, true);
                    if (!E->renderer->count) E->renderer->count++;    
                    return;
                } break;
                default: {
                    if (isprint(c)) {
                        line_push_char(E->renderer->current, c, false);
                        if (E->renderer->count) E->renderer->count++;
                    }
                } break;
            }
        } break;
        default: {} break;
    }
    
    if (!(isalnum(c) || ispunct(c) || isspace(c)))
    {
        if (c == CTRL(KEY_LEFT)) {
            sprintf(E->notification_buffer, "[ CTRL_KEY_LEFT was clicked. ]\n");
        } else if (c == CTRL(KEY_RIGHT)) {
            sprintf(E->notification_buffer, "[ CTRL_KEY_RIGHT was clicked. ]\n");
        } else {
            sprintf(E->notification_buffer, "[ 0x%04x was clicked. ]\n", c);
        }
    }
}




