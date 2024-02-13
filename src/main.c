#include <mi.h>

#define T 0
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
        printf("y: %zu  x: %zu\n", current->y, current->x);
        current = current->next;
    }

    return 0;
}

int editor(int argc, char **argv)
{
    char *program = argv[0], *file;
    WINDOW *win;
    bool deleted_char = false;
    size_t line_count = 0;
    int c = 0;
    Line *lines = Alloc_line_node(0);
    Line *new = NULL;
    Line *current_line = (lines);

    if (argc < 2) {
        printf("USE: %s <file_name>\n", program);
        return 1;
    }

    file = argv[1];
    win = init_editor(file);
    line_count = load_file(file, lines);
    render_lines(lines);
    debugger(win,
                 current_line->x, 
                 current_line->y, 
                 current_line->size); // a helper to display the chords and important info to debug.
    move(0, 0);
    current_line->x = 0;
    current_line->y = 0;
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
                        current_line = current_line->prev; // go back one line.
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
			    if (current_line->prev) {
                    current_line = current_line->prev;
                }
            } break;
			case KEY_DOWN: {
                if (current_line->y < line_count && current_line->next) {
                    current_line = current_line->next;
                }
            } break;
			case KEY_LEFT: {
                if (current_line->x > 0) current_line->x--;
            } break;
			case KEY_RIGHT: {
                if (current_line->x < current_line->size) current_line->x++;
            } break;
			case KEY_HOME:{
                current_line->x = 0;
            } break;
			case KEY_END: {
                if (current_line->size > 0 && current_line->x > 0) current_line->x = current_line->size;
			} break;
            case TAB: {
                // TODO: add 2 tabs to the content of the current_line;
                editor_tabs(current_line);
            } break;
			default: {
                if (c == '\n') {
                    if (current_line->x == current_line->size) {
                        new = Alloc_line_node(current_line->y + 1);
                        if (current_line->next == NULL) {
                            new->prev = current_line;
                            current_line->next = new;
                        } else {
                            Line *next = current_line->next;
                            new->prev = current_line; // correct
                            new->next = next; // correct.
                            current_line->next = new; // correct
                            next->prev = new;
                            new = new->next;
                            // shift
                            while(new) {
                                new->y++;
                                new = new->next;
                            }
                        }
                        current_line = current_line->next;
                        line_count++;
                    } else if (!current_line->x) { // first column.
                        Line *prev = current_line->prev;
                        new = Alloc_line_node(current_line->y);
                        
                        new->next = current_line;
                        
                        if (prev) {
                            prev->next = new;
                            new->prev = prev;
                        }
                        current_line->prev = new;
                        new = current_line;

                        while (new) {
                            new->y++;
                            new = new->next;
                        }

                        if (current_line->y == 1) {
                            lines = current_line->prev;
                        }
                    
                        current_line = current_line->prev;
                        line_count++;
                    }
                } else {
                    line_push_char(current_line, c);
                }

                if (!line_count) line_count++;
            };
        }

        erase();
        render_lines(lines);
        move(current_line->y, current_line->x);
        if (deleted_char) {
            delch();
            deleted_char = false;
        }

        debugger(win,
                 current_line->x, 
                 current_line->y,
                 current_line->size); // a helper to display the chords and important info to debug
    }

    save_file(file, lines, line_count);
	endwin();
	return 0;
}
