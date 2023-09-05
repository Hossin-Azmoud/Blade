#include <mi.h>

MiEditor *editor_new(char *file)
{
	MiEditor *E;

	if (!file)
		return NULL;

	E                     = malloc(sizeof(MiEditor));
	E->cursor             = cursor_new();
	(E->file).handle      = fopen(file, "w+");
	(E->file).lines       = line_construct_new(1);
	(E->file).lines_start = (E->file).lines;
	(E->file).lines_end   = NULL;
	E->window             = NULL;

	return (E);
}

void editor_distroy(MiEditor *E)
{
	cursor_distroy(E->cursor);
	fclose((E->file).handle);
	lines_free((E->file).lines_start);
	delwin(E->window);
    endwin();
    refresh();
	free(E);
}

void editor_write_line_number(MiEditor *E)
{
	(E)->cursor->x_offset = digit_len((E)->cursor->y + 1);
	waddstr(E->window, (E)->cursor->y_cstr);
	
	mvwaddch(E->window,
		  (E)->cursor->y, 
		  (E)->cursor->x + (E)->cursor->x_offset,
		  ' ');

	(E)->cursor->x++;
}

MiEditor *editor_init(char *file)
{
	MiEditor *E;

	if (!file) {
		return NULL;
	}

	E = editor_new(file);

	if ((E->window = initscr()) == NULL ) {
		fprintf(stderr, "Could not initialize MiEditor..\n");
		exit(EXIT_FAILURE);
    }
	
	noecho();
	keypad(E->window, TRUE);
	refresh();
	add_chords_cstr(E->cursor);
	editor_write_line_number(E);
	return E;
}
