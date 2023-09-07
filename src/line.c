#include <mi.h>

Line *line_new()
{
	Line *line = malloc(sizeof(Line));

	{
		line->cap             = DEFUALT_LINE_CAP;
		line->size            = 0;
		line->data            = charlist_new();
		line->first_col       = line->data;
		line->last_col        = line->data;

		line->content         = calloc(DEFUALT_LINE_CAP, 1);

		{
			line->cursor.row  = 0;
			line->cursor.col  = 0;
			line->cursor.row_cstr = calloc(MAX_DIG, 1);

			sprintf(line->cursor.row_cstr, 
				"%ld", 
				line->cursor.row + 1
			);

			line->cursor.col_offset    = digit_len(line->cursor.row + 1) + 1;
		}	
		
		line->next        = NULL;
		line->prev        = NULL;
	}

	return line;
}

void lines_free(Line *line)
{
	Line *curr = line;
	Line *next = NULL;

	while (curr != NULL)
	{
		next = curr->next;
		free(curr->content);
		
		if (curr->first_col)
			charlist_free(curr->first_col);

		if (curr->cursor.row_cstr)
			free(curr->cursor.row_cstr);

		free(curr);
		curr = next;
	}
}

Line *line_next(Line *current)
{
	return (current->next);
}

Line *line_prev(Line *current)
{
	return (current->prev);
}

void line_connect(Line **end)
{
	Line *new = line_new();

	new->cursor.row   = (*end)->cursor.row + 1;
	sprintf(new->cursor.row_cstr, "%ld", new->cursor.row + 1);
	new->cursor.col_offset = digit_len(new->cursor.row + 1) + 1;
	(*end)->next  = new;
	new->prev     = (*end);

	*end          = new;
}
