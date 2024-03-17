#include <mi.h>

static Vec2 vec2() {
    return (Vec2) {.x = 0, .y=0, ._line=NULL};
}

MiEditor *init_editor(char *path)
{
    MiEditor *E = malloc(sizeof(MiEditor));
    char *pathBuff = NULL;
    E->ewindow = init_ncurses_window();
    E->renderer = malloc(sizeof(Lines_renderer));
    editor_load_layout(E);

    // If the caller did not supply a file then we ask him in a seperate screen.
    if (path == NULL) {
        pathBuff = editor_render_startup(E->renderer->win_w / 2, E->renderer->win_h / 2);
    } else {
        pathBuff =  string_dup(path);
    }

    E->fb = new_file_browser(pathBuff);
    free(pathBuff); 
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
    E->renderer->count       = 0;
    E->renderer->start       = E->renderer->origin;
    E->renderer->end         = E->renderer->origin; 
    E->renderer->current     = E->renderer->origin; 
    E->renderer->max_padding = 2;

    // If the path that was passed was a file, or if it does not exist. we assign.
    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
        E->renderer->file_type = get_file_type (E->fb->open_entry_path);
        load_file(E->fb->open_entry_path, E->renderer);
    }

    // Init binding queue.    
    E->binding_queue = (vKeyBindingQueue){0x0};
    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
        editor_refresh(E); 
    } else {
        E->mode = FILEBROWSER;
        render_file_browser(E);
        editor_details(E->renderer, E->fb->open_entry_path, E->mode, E->notification_buffer);
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
    free(E->notification_buffer);
    free(E);
}

void editor_refresh(MiEditor *E)
{
    erase();
    render_lines(E->renderer);
    editor_details(E->renderer, E->fb->open_entry_path, E->mode, E->notification_buffer);
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
            curs_set(1);
    
    switch (E->mode) {
        case FILEBROWSER: {
            render_file_browser(E);
        } break;
        case VISUAL: {
            render_lines(E->renderer);
            E->highlighted_data_length = highlight_until_current_col(E->highlighted_start, E->renderer);
            sprintf(E->notification_buffer, "(*) %d bytes were selected\n", E->highlighted_data_length);
        } break;
        case NORMAL: {
            if (E->binding_queue.size == MAX_KEY_BINDIND) {
                // TODO: Process Key E->binding_queue.
                editor_handle_binding(E->renderer, &E->binding_queue);
                memset(E->binding_queue.keys, 0, E->binding_queue.size);
                E->binding_queue.size = 0;
            }
            if (E->fb->type != DIR__) {
                render_lines(E->renderer);
            } else {
                render_file_browser(E);
            }
        } break;
        default: {
            render_lines(E->renderer);
        } break;
    }
    
    editor_details(E->renderer, E->fb->open_entry_path, E->mode, E->notification_buffer);
    if (strlen(E->notification_buffer) > 0) memset(E->notification_buffer, 0, 1024);

    if (E->mode == FILEBROWSER) return;
    editor_apply_move(E->renderer);
    refresh();
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
                clipboard_save_chunk(E->highlighted_start, E->highlighted_end);
            }

            if (c == KEY_CUT_) {
                // Store into about end of the highlighting in some struct.
                E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
                E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
                E->highlighted_end._line =  E->renderer->current;
                E->mode = NORMAL;
                // Since we are cutting lines so a region of the render_lines, or the lines as referred in the struct will be kille
                // E->renderer->current = clipboard_save_chunk(E->highlighted_start, E->highlighted_end, true);
                
                clipboard_cut_chunk(E->renderer, E->highlighted_start, E->highlighted_end);
            }
        } break;
        
        case NORMAL:  {
            switch (c) {
                case KEY_DOT: {
                    save_file(E->fb->open_entry_path, E->renderer->origin, false);
                    E->fb = realloc_fb(E->fb, "..");
                    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
                        load_file(E->fb->open_entry_path, E->renderer);
                        E->mode = NORMAL;
                    } else {
                        E->mode = FILEBROWSER;
                    }
                } break;
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
                    int saved_bytes = save_file(E->fb->open_entry_path, E->renderer->origin, false);
                    sprintf(E->notification_buffer, "(*) %dL %d bytes were written\n", 
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
}
