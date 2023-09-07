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

	// x: editor->line->col + editor->line->x_offset
	// y: editor->line->number
	editor = editor_init(argv[1]);
	while ( (ch = getch()) != 'q' ) {
		switch ( ch ) {
			case '\n': {
				assert((editor->line->col < editor->line->cap) && "LINE SIZE OUT OF BOUND!");

				editor->line->content[editor->line->col] = ch;
				editor->line->size++;
				editor->line->col++;

				mvwaddch(editor->window,
					editor->line->number,
					editor->line->col + editor->line->x_offset,
					ch
				);

				if (editor->line->next == NULL) {
					// TODO: NEW LINE TO INSERT.
					line_connect(&(editor->line));
					editor->last_line = editor->line;
				} else {
					editor->line = editor->line->next;
				}

				editor_write_line_number(editor);
			} break;
			case KEY_BACKSPACE: {
				if (editor->line->col == 0) {
					// TODO: Return to the prev line.
					if (editor->line->prev == NULL)
						continue;
					
					editor->line = editor->line->prev;
					mvwdelch(editor->window, editor->line->number, editor->line->col + (editor->line->x_offset));
					editor->line->content[editor->line->col--] = 0;
				} else if (editor->line->col > 0) {
					mvwdelch(editor->window, editor->line->number, editor->line->col + (editor->line->x_offset));
					editor->line->content[editor->line->col--] = 0;
				}
			} break;
			case KEY_UP: {
				if (editor->line->prev) {
					editor->line = editor->line->prev;
				}
			} break;
			case KEY_DOWN: {
				if (editor->line->next) {
					editor->line = editor->line->next;
				}
			} break;
			case KEY_LEFT: {
				if (editor->line->col > 0) {
					editor->line->col--;
				}
			} break;
			case KEY_RIGHT: {
				if (editor->line->col < editor->line->size) {
					editor->line->col++;
				}
			} break;
			case KEY_HOME:{
				editor->line      = editor->first_line;
				editor->line->col = 0;
			} break;
			case KEY_END: {
				editor->line->col = editor->line->size;
			} break;
			default: {
				if (editor->line->col == editor->line->size || 1) {
					mvwaddch(editor->window,
						editor->line->number,
						editor->line->col + editor->line->x_offset,
						ch
					);

					editor->line->content[editor->line->col++] = ch;
					editor->line->size++;
					continue;
				}

				for (size_t i = (editor->line->col); i < editor->line->size; i++) {
					editor->line->content[i + 1] = editor->line->content[i];
					mvwaddch(editor->window,
						editor->line->number,
						(i + editor->line->x_offset),
						editor->line->content[i]
					);
				}
 
				editor->line->content[editor->line->col++] = ch;
				editor->line->size++;
				mvwaddch(editor->window,
					editor->line->number,
					editor->line->col + editor->line->x_offset, 
					ch);
			} break;
		}

		move(editor->line->number, editor->line->col + editor->line->x_offset);
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
