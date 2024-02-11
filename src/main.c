#include <mi.h>

int main(int argc, char **argv)
{
    char *program = argv[0], *file;
    WINDOW *win;
    bool deleted_char = false;
    size_t line_count = 0;
    int c = 0;
    // FILE *Stream;
    Line lines[LINE_SZ] = { 0 }; // 256 lines that can store 256 ^ 2 bytes.
    Line *prev, *next = NULL;
    Line *current_line = (lines);

    if (argc < 2) {

        printf("USE: %s <file_name>\n", program);
        return 1;
    }

    file = argv[1];
    win = init_editor(file);
    line_count = load_file(file, lines);
    render_lines(lines, line_count);
    debugger(win,
                 current_line->x, 
                 current_line->y, 
                 current_line->size); // a helper to display the chords and important info to debug.
    move(0, 0);
    
    while ((c = getch()) != KEY_F(1)) {
        switch (c) {
			case KEY_BACKSPACE: {
				// TODO
                if (current_line->x > 0) {
                    current_line->content[current_line->x] = 0;
                    current_line->x--;
                    current_line->size--;
                } else {
                    if (current_line->y > 0) {
                        current_line = lines + current_line->y - 1;
                        current_line->content[current_line->x] = 0;
                        if (current_line->x) {
                            current_line->x--;
                            current_line->size--;
                        }
                    } else {
                        current_line->content[current_line->x] = 0;
                    }
                }
                deleted_char = true;
			} break;
			case KEY_UP: {
			    if (current_line->y > 0) {
                    current_line = lines + current_line->y - 1;
                }
            } break;
			case KEY_DOWN: {
			    // TODO
                if (current_line->y < line_count) {
                    current_line = lines + current_line->y + 1;
                }
            } break;
			case KEY_LEFT: {
            	// TODO
                if (current_line->x > 0) current_line->x--;
            } break;
			case KEY_RIGHT: {
			    // TODO
                if (current_line->x < current_line->size) current_line->x++;
            } break;
			case KEY_HOME:{
			    // TODO
                current_line->x = 0;
            } break;
			case KEY_END: {
                // TODO
                if (current_line->size > 0 && current_line->x > 0) current_line->x = current_line->size - 1;
			} break;
            case '\t': {
                // TODO
            } break;
			default: {
                if (c == '\n') {
                    if (current_line->x == current_line->size) {
                        prev = current_line;
                        current_line = (prev + 1);
                        current_line->y = (prev->y + 1);
                        line_count++;
                    } else {
                        // TODO: Move memory from the current line to (y + 1)(NEWLINE..)
                         
                        shift
                        
                    }
                } else {
                    memmove((current_line->content + current_line->x + 1), 
                                (current_line->content + current_line->x),
                                current_line->size - current_line->x);
                    current_line->content[current_line->x] = c;
                    current_line->x++;
                    current_line->size++;
                }

                if (!line_count) line_count++;
            };
        }

        render_lines(lines, line_count);
        move(current_line->y, current_line->x);
        if (deleted_char) {
            delch();
            deleted_char = false;
        }

        debugger(win,
                 current_line->x, 
                 current_line->y,
                 current_line->size); // a helper to display the chords and important info to debug.
        refresh();
    }

    save_file(file, lines, line_count);
	endwin();
	return 0;
}
