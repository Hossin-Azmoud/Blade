#include <mi.h>
/***
 *
 * TODO:
 * - Handle memory moves when the cursor is on a line and a x>0 column. (*) Done
 * - Handle pages and pages Up/Down keys. (*) Done
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
    Line *head = Alloc_line_node(LINE_SZ);
    Line *current = head;
    while (current) {
        printf("y: %d  x: %d\n", current->y, current->x);
        current = current->next;
    }
    return 0;
}

int editor(int argc, char **argv)
{
    char *program = argv[0], *file;
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

    if (argc < 2) {
        printf("USE: %s <file_name>\n", program);
        return 1;
    }

    file = argv[1];
    win = init_editor();
    getmaxyx(win, 
             line_ren->win_h, 
             line_ren->win_w);
    erase();
    line_ren->count = load_file(file, line_ren);    
    // NOTE: fix the renderer >~<
    render_lines(line_ren);
#ifdef DEBUG
    editor_details(line_ren, file);
#endif /* ifdef DEBUG */

    editor_apply_move(line_ren);
    while ((c = getch()) != CTRL('s') && c != KEY_F(1)) {
        switch (c) {
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
            case KEY_F(2): {
                exit_pressed = true;
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

#ifdef DEBUG
        editor_details(line_ren, file);
#endif /* ifdef DEBUG */
    editor_apply_move(line_ren);
    }

    endwin();
    if (!exit_pressed) {
        save_file(file, line_ren->origin);
        return 0;
    }
	
    free_lines(line_ren->origin);
	return 0;
}
