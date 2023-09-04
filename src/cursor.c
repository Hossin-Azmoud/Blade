#include <mi.h>


Cursor *cursor_new()
{
	Cursor *C = malloc(sizeof(Cursor));

	{
		C->x_offset = 0; // Place for the first digit which is 0->9 and a space.

		C->x        = C->x_offset;
		C->y        = C->x_offset;
		C->x_max    = C->x_offset;
		C->y_max    = C->x_offset;
	
		(C)->x_cstr = calloc(MAX_DIG, 1);
		(C)->y_cstr = calloc(MAX_DIG, 1);
		add_chords_cstr(C);
	}

	return C;
}

void cursor_distroy(Cursor *C)
{
	free((C)->x_cstr);
	free((C)->y_cstr);
	free(C);
}

void add_chords_cstr(Cursor *C)
{
	sprintf((C)->x_cstr, "%d", C->x + 1);
	sprintf((C)->y_cstr, "%d", C->y + 1);
}
