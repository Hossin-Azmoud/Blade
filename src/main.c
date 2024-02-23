#include <mi.h>
#include <logger.h>

/***
 *
 * TODO:
 * - Handle memory moves when the cursor is on a line and a x>0 column. (*) Done
 * - Handle pages and pages Up/Down keys. (*) Done
 * - Handle backspace left age case!
 ***/
#define T 0
// #define DEBUG
int editor(int argc, char **argv);
int test();

int main(int argc, char **argv) {
    if (T) return test();
    return editor(argc, argv);
}

int test() {
    return 0;
}

int editor(int argc, char **argv)
{
    open_logger();

    Vec2 copy_start = { 0 }; 
    Vec2 copy_end   = { 0 };

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
        .max_padding=0
    };

    int highlighted_bytes = 0;
    Lines_renderer *line_ren =  &line_ren_raw;

    bool deleted_char = false;
    int c = 0;
    line_ren->origin  = Alloc_line_node(0);

    line_ren->start   = line_ren->origin;
    line_ren->end     = line_ren->origin; 
    line_ren->current = line_ren->origin; 

    bool exit_pressed = false;   
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

    erase();   
    line_ren->count = load_file(file, line_ren);

    render_lines(line_ren);
    editor_details(line_ren, file, mode, notification_buffer);


    editor_apply_move(line_ren);

    while ((c = getch()) != KEY_F(1)) {
        
        switch (c) {
			case KEY_F(2): {
                exit_pressed = true;
            } break;
            case CTRL('v'): {
                mode = VISUAL;
                // we mark the chords of the start position!
                copy_start.x = line_ren->current->x;
                copy_start.y = line_ren->current->y;
                copy_start._line = line_ren->current;
            } break;
            case CTRL('n'): {
                mode = NORMAL;
            } break;
            case CTRL('t'): {
                mode = INSERT;
            } break;
            case CTRL('x'): {
                int saved_bytes = save_file(file, line_ren->origin, false);
                sprintf(notification_buffer, "[ %dL %d bytes were written ]\n", line_ren->count, saved_bytes);
            } break;
            case KEY_COPY_: {
                if (mode == VISUAL) {
                    // Store into about end of the highlighting in some struct.
                    copy_end.x = line_ren->current->x;
                    copy_end.y = line_ren->current->y;
                    copy_end._line = line_ren->current;

                    mode = NORMAL;
                    sprintf(notification_buffer, "[ (%d, %d) -> (%d, %d)]\n", 
                            copy_start.x, copy_start.y,
                            copy_end.x, copy_end.y);
                }
            } break;
            case KEY_PASTE_: {
                // TODO. editor_paste_content
                editor_paste_content(copy_start, copy_end, line_ren);
            } break;
            case KEY_BACKSPACE: { 
                if (mode == INSERT) {
                    editor_backspace(line_ren);
                    deleted_char = true;
                } else {
                    editor_up(line_ren);
                }
			} break;
			case KEY_UP: {
			    editor_up(line_ren); 
            } break;
			case KEY_DOWN: {                
                editor_down(line_ren);
            } break;
			case KEY_LEFT: {
                editor_left(line_ren);
            } break;
			case KEY_RIGHT: {
                editor_right(line_ren);
            } break;
            case TAB: {
                if (mode == INSERT) editor_tabs(line_ren->current);
            } break;
			case KEY_HOME:{
                line_ren->current->x = 0;
            } break;
			case KEY_END: {
                if (line_ren->current->size > 0) 
                    line_ren->current->x = line_ren->current->size;
			} break;
			default: {
                if (c == '\n') {
                    if (mode == VISUAL || mode == NORMAL) {
                        editor_down(line_ren);
                    } else if (mode == INSERT) {
                        editor_new_line(line_ren);
                        if (!line_ren->count) 
                            line_ren->count++;    
                    }
                    goto RENDER;
                }
                if (mode == INSERT) {
                    line_push_char(
                        line_ren->current, 
                        c);
                    if (!line_ren->count) line_ren->count++;
                }
                // TODO: make some other commands for the other mode insted of writing text!
            };
        }

    RENDER:
        erase();

        render_lines(line_ren);
        
        // Highlight if the current context is VISUAL.
        if (mode == VISUAL) {
            highlighted_bytes = highlight_until_current_col (copy_start, line_ren);
            sprintf(notification_buffer, "[ %d bytes were Highlighted]\n", highlighted_bytes);
        }

        if (deleted_char) {
            delch();
            deleted_char = false;
        }
    
        if (exit_pressed) {
            break;
        }

        editor_details(line_ren, file, mode, notification_buffer);

        if (strlen(notification_buffer) > 0) 
            memset(notification_buffer, 0, strlen(notification_buffer));

        editor_apply_move(line_ren);
    }

    if (!exit_pressed) {
        endwin();
        save_file(file, line_ren->origin, true);
        
        close_logger();
        return 0;
    }


EXIT_AND_RELEASE_RESOURCES:
    endwin();
    free_lines(line_ren->origin);
	close_logger();
    return 0;
}
