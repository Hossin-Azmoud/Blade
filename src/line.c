#include <mi.h>

Line *line_new()
{
	Line *line = malloc(sizeof(Line));

	{
		line->cap         = DEFUALT_LINE_CAP;
		line->size        = 0;
		line->content     = calloc(DEFUALT_LINE_CAP, 1);
		line->number      = 0;
		line->number_cstr = calloc(MAX_DIG, 1);
		sprintf(line->number_cstr, 
		  "%ld", 
		  line->number + 1
		);
		line->x_offset    = digit_len(line->number + 1) + 1;
		line->col         = 0;
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

		if (curr->number_cstr)
			free(curr->number_cstr);

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

	new->number   = (*end)->number + 1;
	sprintf(new->number_cstr, "%ld", new->number + 1);
	new->x_offset = digit_len(new->number + 1) + 1;
	(*end)->next  = new;
	new->prev     = (*end);

	*end          = new;
}
