#ifndef MI_H
#define MI_H
#include <stdlib.h>
#include <assert.h>
#include <curses.h>
#include <string.h>
#include <ui.h>
#define MAX_DIG    32
#define MAXX_LINES 2048
#define LOG \
	do { \
		printf("[LOGGER] %s:%d:%s\n", \
			__FILE__, __LINE__, __func__); \
	} while(0); \

typedef struct cursor_s {
	int x, x_max, y, y_max, x_offset;
	char *x_cstr;
	char *y_cstr;
} Cursor;

typedef struct line_s {
	size_t size;
	char   *content;
} Line;

typedef struct editor {
	Cursor *cursor;
	Line   lines[MAXX_LINES];
	WINDOW *window;
	FILE   *fhandle;
} MiEditor;

// CURSOR
Cursor *cursor_new();
void cursor_distroy(Cursor *C);
void add_chords_cstr(Cursor *C);

// EDITOR
MiEditor *editor_new(char *file);
void editor_distroy(MiEditor *E);
void editor_write_line_number(MiEditor *E);
MiEditor *editor_init(char *file);

// STD
size_t digit_len(size_t n);

#endif  // MI_H
