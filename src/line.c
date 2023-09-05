#include <mi.h>

Line *line_new()
{
	Line *line = malloc(sizeof(Line))

	{
		line->cap      = DEFUALT_LINE_CAP;
		line->size     = 0;
		line->content  = malloc(DEFUALT_LINE_CAP);
		line->number   = 0;
		line->x_offset = 0;
		line->next     = NULL;
		line->prev     = NULL;
	}

	return line;
}

Line *line_construct_new(size_t number)
{
	Line *line     = line_new();
	line->number   = number;
	line->x_offset = digit_len(number + 1);
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
		free(curr);
		curr = next;
	}
}
