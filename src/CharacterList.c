#include <mi.h>

CharList *charlist_new()
{
	CharList *ls = malloc(sizeof(ls));

	ls->data = 0;
	ls->next = NULL;
	ls->prev = NULL;

	return (ls);
}

void charlist_connect(CharList **end)
{
	(void) end;
	LOG;
}

void charlist_free(CharList *head)
{
	CharList *curr = head;
	CharList *next = NULL;

	while (curr != NULL)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}
