#include <mi.h>
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
            case KEY_BACKSPACE: {
                editor_backspace(line_ren);
                deleted_char = true;
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
                editor_tabs(line_ren->current);
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
                    editor_new_line(line_ren);
                    if (!line_ren->count) 
                        line_ren->count++;
                    goto RENDER;
                } 
                line_push_char(line_ren->current, c);
                if (!line_ren->count) line_ren->count++;
            };
        }

    RENDER:
        erase();
        render_lines(line_ren);

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
        return 0;
    }


EXIT_AND_RELEASE_RESOURCES:
    endwin();
    free_lines(line_ren->origin);
	return 0;
}
