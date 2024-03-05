#include <mi.h>

#define T 0
// #define DEBUG
int editor(int argc, char **argv);
int test();

int main(int argc, char **argv) {
    if (T) return test();
    int ret = editor(argc, argv);
    CLIPBOARD_FREE();
    close_logger();
    return ret;
}

void print_token_list(TokenList *list)
{
    
    for (int i = 0; i < list->size; ++i) {
        printf("(s: %d e: %d) - %s\n", 
           list->_list[i].xstart, 
           list->_list[i].xend,
           get_token_kind_s(list->_list[i].kind)
        );
    }
}

int test() {
    open_logger();
    const char *src = "function main(name) { console.log(\"Hello: \"name); }";
    Line *line = Alloc_line_node(0);
    strcpy(line->content, src);
    line->size = strlen(src);
    
    retokenize_line(line, JS);
    print_token_list(&line->token_list);
    free_lines(line);
    return 0;
}

int editor(int argc, char **argv)
{
    open_logger();
    // curs_set(1);
    Vec2 copy_start = { 0 }; 
    Vec2 copy_end   = { 0 };
    vKeyBindingQueue binding = { 0 };
    char *file;
    editorMode mode = NORMAL;
    char notification_buffer[1024] = {0};
    WINDOW *win = NULL;
    Lines_renderer line_ren_raw = {
        .origin=NULL, 
        .start=NULL, 
        .end=NULL, 
        .current=NULL, 
        .count=0,
        .max_padding=2
    };

    int highlighted_bytes = 0;
    Lines_renderer *line_ren =  &line_ren_raw;
    bool deleted_char = false;
    int c = 0;
    bool exit_pressed = false;
    line_ren->origin  = Alloc_line_node(0);
    line_ren->start   = line_ren->origin;
    line_ren->end     = line_ren->origin; 
    line_ren->current = line_ren->origin; 
    
    
    win = init_editor();    
    getmaxyx(win, 
             line_ren->win_h, 
             line_ren->win_w); 
    if (argc < 2) {
        file = editor_render_startup(line_ren->win_w / 2, line_ren->win_h / 2);
        if (file == NULL) {
            // user cancelled! we quit.
            goto EXIT_AND_RELEASE_RESOURCES;
        }
    } else {
        file = argv[1];
    }
    line_ren->script_type = get_script_type (file);
    erase();   
    line_ren->count = load_file(file, line_ren);
    render_lines(line_ren);
    editor_details(line_ren, file, mode, notification_buffer);
    editor_apply_move(line_ren);
    // TODO: void compute_layout (Lines_renderer *line_ren);
    // it adjusts the start and end of the layout so it can be suitable with the current row count
    while ((c = getch()) != KEY_F(1)) {
        getmaxyx(win, 
             line_ren->win_h, 
             line_ren->win_w); 
        // compute_layout(line_ren);
        if (is_move(c)) {
            handle_move(c, line_ren);
            goto RENDER;
        }

        // Globals.
        switch (c) {
            case KEY_F(2): {
                exit_pressed = true;
            } break;
        }
        // Switch To NORMAL Mode using escape
        if (c == ESC) {
            mode = NORMAL;
            goto RENDER;
        }

        
        // Actions that depend on the mode.
        switch (mode) {
            case VISUAL: {
                if (c == KEY_COPY_) {
                    // Store into about end of the highlighting in some struct.
                    copy_end.x = line_ren->current->x;
                    copy_end.y = line_ren->current->y;
                    copy_end._line = line_ren->current;
                    mode = NORMAL;
                    clipboard_save_chunk(copy_start, copy_end);
                }
            } break;
            
            case NORMAL: {
                switch (c) {
                    case KEY_PASTE_: {
                        // TODO: Make clipboard be synced with the VISUAL mode clipboard_
                        editor_push_data_from_clip(line_ren);
                    } break;
                    case KEY_INSERT_: {
                        mode = INSERT;
                    } break;
                    case KEY_VISUAL_: {
                        // we mark the chords of the start position!
                        mode = VISUAL;
                        copy_start.x = line_ren->current->x;
                        copy_start.y = line_ren->current->y;
                        copy_start._line = line_ren->current;
                    } break;
                    case KEY_SAVE_: {
                        int saved_bytes = save_file(file, line_ren->origin, false);
                        sprintf(notification_buffer, "[ %dL %d bytes were written ]\n", line_ren->count, saved_bytes);
                    } break;
                    default: {
                        if (binding.size < MAX_KEY_BINDIND)
                            binding.keys[binding.size++] = (char) c;
                            // TODO: Make binding copy whole line with `yy`, and delete whole line with `dd`
                        if (binding.size == MAX_KEY_BINDIND) {
                            // TODO: Process Key binding.
                            editor_handle_binding(line_ren, &binding);
                            binding.size = 0;
                        }
                    } break;
                }
            } break;
            case INSERT: {
                switch (c) {
                    case TAB: {
                        editor_tabs(line_ren->current);
                    } break;
                    case KEY_BACKSPACE: { 
                        editor_backspace(line_ren);
                        deleted_char = true;
			        } break;
                    case KEY_DEL: {
                        // TODO: Implement this.
                        editor_dl(line_ren->current);
                        deleted_char = true;
                    } break;		
                    case NL: {
                        editor_new_line(line_ren, true);
                        if (!line_ren->count) line_ren->count++;    
                        goto RENDER;
                    } break;
                    default: {
                        line_push_char(line_ren->current, c, false);
                        if (!line_ren->count) line_ren->count++;
                    } break;
                }
            } break;
            default: {} break;
        }
        
        if (!(isalnum(c) || ispunct(c) || isspace(c)))
        {
            if (c == CTRL(KEY_LEFT)) {
                sprintf(notification_buffer, "[ CTRL_KEY_LEFT was clicked. ]\n");
            } else if (c == CTRL(KEY_RIGHT)) {
                sprintf(notification_buffer, "[ CTRL_KEY_RIGHT was clicked. ]\n");
            } else {
                sprintf(notification_buffer, "[ 0x%04x was clicked. ]\n", c);
            }
        }

    RENDER:
        erase();
        
        if (deleted_char) {
            delch();
            deleted_char = false;
        }
        render_lines(line_ren);

        // Highlight if the current context is VISUAL.
        if (mode == VISUAL) {
            highlighted_bytes = highlight_until_current_col (copy_start, line_ren);
            sprintf(notification_buffer, "[ %d bytes were Highlighted]\n", highlighted_bytes);
        }

    
        if (exit_pressed) {
            break;
        }

        editor_details(line_ren, file, mode, notification_buffer);
        if (strlen(notification_buffer) > 0) memset(notification_buffer, 0, strlen(notification_buffer));
        editor_apply_move(line_ren);
        refresh();
    }

    if (!exit_pressed) {
        endwin();
        save_file(file, line_ren->origin, true);
        return 0;
    }


EXIT_AND_RELEASE_RESOURCES:
    endwin();
    free_lines(line_ren->origin);
    return 0;
}

