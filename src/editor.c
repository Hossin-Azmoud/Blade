#include <mi.h>

MiEditor *editor_new(char *file)
{
	MiEditor *E;

	if (!file)
		return NULL;

	E          = malloc(sizeof(MiEditor));
	E->cursor  = cursor_new();
	E->fhandle = fopen(file, "w+");

	for (int i = 0; i < MAXX_LINES; ++i) {
		E->lines[i].size    = 0;
		E->lines[i].content = NULL;
	}

	E->window = NULL;
	return (E);
}

void editor_distroy(MiEditor *E)
{
	cursor_distroy(E->cursor);
	fclose(E->fhandle);
	for (int j = 0; j < MAXX_LINES; ++j) {
		free(E->lines[j].content);
		// If it is dynamically allocated then.
		// free(E->lines[j]);
	}

	delwin(E->window);
    endwin();
    refresh();
	free(E);
}

void editor_write_line_number(MiEditor *E)
{
	// waddstr(E->window, STD_YELLOW);
	waddstr(E->window, (E)->cursor->y_cstr);
	(E)->cursor->x        += digit_len((E)->cursor->y + 1);

	mvwaddch(E->window,
		  (E)->cursor->y, 
		  (E)->cursor->x, 
		  ' ');

	// waddstr(E->window, STD_NRM);
	(E)->cursor->x_offset++;
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
