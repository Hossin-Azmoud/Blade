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
	
	// x: editor->line->cursor.col + editor->line->cursor.col_offset
	// y: editor->line->cursor.row

	editor = editor_init(argv[1]);
	while ( (ch = getch()) != 'q' ) {
		switch ( ch ) {
			case KEY_BACKSPACE: {
				editor_back_space(editor);
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
				if (editor->line->cursor.col > 0) {
					editor->line->cursor.col--;
				}
			} break;
			case KEY_RIGHT: {
				if (editor->line->cursor.col < editor->line->size) {
					editor->line->cursor.col++;
				}
			} break;
			case KEY_HOME:{
				editor->line      = editor->first_line;
				editor->line->cursor.col = 0;
			} break;
			case KEY_END: {
				editor->line->cursor.col = editor->line->size;
			} break;
			default: editor_push_char(editor, ch); // NOTE: - we need only to add the char.
		}

		move(editor->line->cursor.row, 
			editor->line->cursor.col + editor->line->cursor.col_offset);
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
