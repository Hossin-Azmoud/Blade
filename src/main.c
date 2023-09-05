#include <mi.h>

int main(int argc, char *argv[])
{
    int ch = 0;
	MiEditor *editor = NULL;

	if (argc < 2) {
		fprintf(stderr, "\n");
		fprintf(stderr, "    USAGE: mi <file_name>\n");
		fprintf(stderr, "    [ERROR] No input file was specified :\'(\n");
		fprintf(stderr, "\n");
		exit(1);
	}

	editor = editor_init(argv[1]);
	while ( (ch = getch()) != 'q' ) {		
		switch ( ch ) {
			case '\n': {
				mvwaddch(editor->window, 
					editor->cursor->y,
					editor->cursor->x + (editor->cursor->x_offset),
					ch);

				if (editor->cursor->y > editor->cursor->y_max) 
					editor->cursor->y_max = editor->cursor->y;

				(editor->cursor)->y++;
				(editor->cursor)->x = 0;
				add_chords_cstr(editor->cursor);
				editor_write_line_number(editor);
			} break;
			case KEY_BACKSPACE: {

				if (editor->cursor->x > (editor->cursor->x_offset)) {
					editor->cursor->x--;
				} else {
					if (editor->cursor->y)
						editor->cursor->y--;
				}

				mvwdelch(editor->window, editor->cursor->y, editor->cursor->x + (editor->cursor->x_offset));
			} break;
			case KEY_UP: {
				if (editor->cursor->y > 0)
					editor->cursor->y--;
			} break;
			case KEY_DOWN: {
				if (editor->cursor->y <= editor->cursor->y_max && editor->cursor->y_max)
					editor->cursor->y++;
			} break;
			case KEY_LEFT: {
				if (editor->cursor->x > (editor->cursor->x_offset)) {
					editor->cursor->x--;
				}
			} break;
			case KEY_RIGHT: {
				if (editor->cursor->x < editor->cursor->x_max)
					editor->cursor->x++;
			} break;
			case KEY_HOME:{
				editor->cursor->x = 0;
				editor->cursor->y = 0;
			} break;
			case KEY_END: {
				editor->cursor->x = editor->cursor->x_max;
			} break;
			default: {
				mvwaddch(editor->window,
					editor->cursor->y,
					editor->cursor->x + (editor->cursor->x_offset), 
					ch);

				editor->cursor->x++;
				if (editor->cursor->x > editor->cursor->x_max)
					editor->cursor->x_max = editor->cursor->x;
			} break;
		}

		move(editor->cursor->y, editor->cursor->x + (editor->cursor->x_offset));
    }
	
	editor_distroy(editor);
	return EXIT_SUCCESS;
}

void ttd(size_t input, size_t expected)
{
	size_t found = digit_len(input);
	printf("[%s][i: %ld] found: %ld expected: %ld\n", (found == expected ) ? "OK" : "ERR", input, found, expected);
}

int main2()
{
	ttd(199, 3);	
	ttd(1, 1);	
	ttd(1000, 4);
	ttd(0, 1);
	ttd(10, 2);
	return (0);
}


