#include <mi.h>

MiEditor *editor_new(char *file)
{
	MiEditor *E;

	if (!file)
		return NULL;

	E               = malloc(sizeof(MiEditor));
	E->handle       = fopen(file, "w+");
	E->line         = line_new();
	E->first_line   = E->line;
	E->last_line    = E->line;
	E->window       = NULL;

	return (E);
}

void editor_distroy(MiEditor *E)
{
	Line *ptr = E->first_line;

	delwin(E->window);
    endwin();
    refresh();

	{
		// SAVING THE FILE.
		for (;ptr != NULL; (ptr = ptr->next))
			fprintf(E->handle, "%s", ptr->content);

		fclose(E->handle);
	}
	
	free(E);
}

void editor_write_line_number(MiEditor *E)
{
	waddstr(E->window,
		E->line->cursor.row_cstr
	);

	mvwaddch(E->window,
		(E)->line->cursor.row,
		E->line->cursor.col + E->line->cursor.col_offset,
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

void editor_render_char(MiEditor *E, char c)
{
	mvwaddch((E)->window,
		(E)->line->cursor.row,
		(E)->line->cursor.col + (E)->line->cursor.col_offset,
		c
	);
}

void editor_new_line(MiEditor *E)
{
	assert((E->line->cursor.col < E->line->cap) && "LINE SIZE OUT OF BOUND!");

	E->line->content[E->line->cursor.col] = NL; // \n
	E->line->size++; // ...
	E->line->cursor.col++;
	
	editor_render_char(E, NL);

	if (E->line->next == NULL) {
		// TODO: NEW LINE TO INSERT.
		line_connect(&(E->line));
		E->last_line = E->line;
	} else {
		// TODO: CONNECT A NEW LINE WITH THE LATEST LINE.
		E->line = E->line->next;
	}

	editor_write_line_number(E);
}

void editor_push_char(MiEditor *E, char c)
{
	if (c == NL) {
		editor_new_line(E);
		return;
	}

	if ((E)->line->cursor.col == (E)->line->size || 1) {
		editor_render_char(E, c);
		(E)->line->content[(E)->line->cursor.col++] = c;
		(E)->line->size++;
		return;
	}

	for (size_t i = ((E)->line->cursor.col); i < (E)->line->size; i++) {
		(E)->line->content[i + 1] = (E)->line->content[i];
		editor_render_char(E, c);
	}

	(E)->line->content[(E)->line->cursor.col++] = c;
	(E)->line->size++;
	editor_render_char(E, c);
}

void editor_pop_char(MiEditor *E)
{
	mvwdelch(E->window, 
		  E->line->cursor.row, 
		  E->line->cursor.col + (E->line->cursor.col_offset));
}

void editor_back_space(MiEditor *E)
{
	if (E->line->cursor.col == 0) {
		if (E->line->prev == NULL) {
			E->line->content[E->line->cursor.col] = 0;
			return;
		}

		E->line = E->line->prev;
		E->line->content[E->line->cursor.col] = 0;
	} else if (E->line->cursor.col > 0) {
		E->line->cursor.col--;
		E->line->content[E->line->cursor.col] = 0;
	}

	if (E->line->size) {
		E->line->size--;
		editor_pop_char(E);
	}
}
