#include <mi.h>

MiEditor *editor_new(char *file)
{
	MiEditor *E;

	if (!file)
		return NULL;

	E               = malloc(sizeof(MiEditor));
	E->cursor       = cursor_new();
	E->handle       = fopen(file, "w+");
	E->line         = line_new();
	E->first_line   = E->line;
	E->last_line    = NULL;
	E->window       = NULL;

	return (E);
}

void editor_distroy(MiEditor *E)
{
	Line *ptr = E->first_line;

	delwin(E->window);
    endwin();
    refresh();
	cursor_distroy(E->cursor);

	{
		// SAVING THE FILE.
		for (;ptr != NULL; (ptr = ptr->next))
			fprintf(E->handle, "%s",   ptr->content);	

		fclose(E->handle);
	}

	free(E);
}

void editor_write_line_number(MiEditor *E)
{
	waddstr(E->window,
		E->line->number_cstr
	);

	mvwaddch(E->window,
		(E)->line->number,
		E->line->col + E->line->x_offset,
		' '
	);
}

MiEditor *editor_init(char *file)
{
	MiEditor *E;

	if (!file) {
		return NULL;
	}

	E = editor_new(file);
	E->window = initscr();

	if (E->window == NULL ) {
		fprintf(stderr, "Could not initialize MiEditor..\n");
		exit(EXIT_FAILURE);
    }
	
	noecho();
	keypad(E->window, TRUE);
	refresh();
	editor_write_line_number(E);

	return E;
}
