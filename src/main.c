#include <mi.h>

#define T 0
// #define DEBUG
int editor(int argc, char **argv);
int test();
int _editor_test(char **argv);

int main(int argc, char **argv) {
    (void) argc; 
    if (T) return test();
    int ret = _editor_test(argv);
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

int _editor_test(char **argv)
{
    open_logger();
    MiEditor *E = init_editor(argv[1]);
    int c = 0;
    if (E == NULL) {
        goto EXIT_AND_RELEASE_RESOURCES;
    }
    
    while ((c = getch()) != KEY_F(1)) {
        editor_load_layout(E);

        if (is_move(c)) {
            handle_move(c, E->renderer);
            goto RENDER;
        }

        // Globals.
        switch (c) {
            case KEY_F(2): {
                E->exit_pressed = true;
            } break;
        }

        // Switch To NORMAL Mode using escape
        if (c == ESC) {
            E->mode = NORMAL;
            goto RENDER;
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
                        if (E->binding_queue.size < MAX_KEY_BINDIND)
                            E->binding_queue.keys[E->binding_queue.size++] = (char) c;
                            // TODO: Make binding copy whole line with `yy`, and delete whole line with `dd`
                        if (E->binding_queue.size == MAX_KEY_BINDIND) {
                            // TODO: Process Key E->binding_queue.
                            editor_handle_binding(E->renderer, &E->binding_queue);
                            E->binding_queue.size = 0;
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
                        // TODO: Implement this.
                        editor_dl(E->renderer->current);
                        E->char_deleted = true;
                    } break;		
                    case NL: {
                        editor_new_line(E->renderer, true);
                        if (!E->renderer->count) E->renderer->count++;    
                        goto RENDER;
                    } break;
                    default: {
                        line_push_char(E->renderer->current, c, false);
                        if (E->renderer->count) E->renderer->count++;
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

    RENDER:
        
        if (E->char_deleted) {
            delch();
            E->char_deleted = false;
        }
 
        // Highlight if the current context is VISUAL.
        if (E->mode == VISUAL) {
            E->highlighted_data_length = highlight_until_current_col(E->highlighted_start, E->renderer);
            sprintf(E->notification_buffer, "[ %d bytes were Highlighted]\n", E->highlighted_data_length);
        }
    
        if (E->exit_pressed) {
            break;
        }
        editor_refresh(E);
        if (strlen(E->notification_buffer) > 0) memset(E->notification_buffer, 0, 1024);
    }

    if (!E->exit_pressed) {
        endwin();
        save_file(
            E->file, 
            E->renderer->origin, 
            true);
        return 0;
    }

EXIT_AND_RELEASE_RESOURCES:
    endwin();
    release_editor(E);
    return 0;
}
