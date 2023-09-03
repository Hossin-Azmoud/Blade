#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <string.h>

FILE *fp = NULL;
#define MAX_DIG 32
typedef struct cursor {
	int x; // Cols
	int y; // Rows
	int x_cstrln, y_cstrln;
	char *x_cstr;
	char *y_cstr;
} Cursor;

void add_chords_cstr(Cursor *C)
{
	C->x_cstrln = sprintf((C)->x_cstr, "%d", C->x + 1);
	C->y_cstrln = sprintf((C)->y_cstr, "%d", C->y + 1);
}

Cursor *init_cursor()
{
	Cursor *C = malloc(sizeof(Cursor));

	C->x = 0;
	C->y = 0;

	(C)->x_cstr = calloc(MAX_DIG, 1);
	(C)->y_cstr = calloc(MAX_DIG, 1);

	add_chords_cstr(C);
	return C;
}

void deinit_cursor(Cursor *C)
{
	free((C)->x_cstr);
	free((C)->y_cstr);
	free(C);
}

void _write_line(Cursor *C, WINDOW *win)
{
	int it = 0;
	C->x += waddstr(win, C->y_cstr);
	mvwaddch(win, (C)->y, (C)->x, ' ');
	C->x++;
}

int main(void)
{
	WINDOW * mainwin;
    int      ch, x_max = 0, y_max = 0;

	fp = fopen("file.txt", "w");
	Cursor *C = init_cursor();

    if ( (mainwin = initscr()) == NULL ) {
		fprintf(stderr, "Error initialising ncurses.\n");
		exit(EXIT_FAILURE);
    }

    noecho();
    keypad(mainwin, TRUE);
	refresh();
	add_chords_cstr(C);
	(C)->x = C->x_cstrln + 1;
	_write_line(C, mainwin);

	while ( (ch = getch()) != 'q' ) {		
		switch ( ch ) {
			case '\n': {
				mvwaddch(mainwin,((C)->y),((C)->x),(ch));
				(C)->x = C->x_cstrln + 1;
				(C)->y++;
				if (C->y > y_max)
					y_max = C->y;
				add_chords_cstr(C);
				_write_line(C, mainwin);
			} break;
			case KEY_BACKSPACE: {
				if ((C)->x - 1) {
					(C)->x--;
				} else {
					if ((C)->y)
						(C)->y--;
				}
				mvwdelch(mainwin, (C)->y, (C)->x);
			} break;
			case KEY_UP: {
				if ( C->y > 0)
					C->y--;
			} break;
			case KEY_DOWN: {
				if (C->y <= y_max)
					C->y++;
			} break;
			case KEY_LEFT: {
				if (C->x > 0 )
					C->x--;
			} break;
			case KEY_RIGHT: {
				if (C->x + 1 <= x_max)
					C->x++;
			} break;
			case KEY_HOME:{
				C->x = 0;
				C->y = 0;
			}break;
			case KEY_END: {
				C->x = x_max;
				C->y = y_max;
				
			} break;
			default: {
				mvwaddch(mainwin,((C)->y),((C)->x),(ch));
				(C)->x++;
				if (C->x > x_max)
					x_max = C->x;
			} break;
		}
		move(C->y, C->x);
    }
	
	delwin(mainwin);
    endwin();
    refresh();
    deinit_cursor(C);
	fclose(fp);

	return EXIT_SUCCESS;
}
