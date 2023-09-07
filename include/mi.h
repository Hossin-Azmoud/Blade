#ifndef MI_H
#define MI_H
#include <stdlib.h>
#include <assert.h>
#include <curses.h>
#include <string.h>
#define MAX_DIG    32
#define DEFUALT_LINE_CAP 1024
#define LOG \
	do { \
		printf("[LOGGER] %s:%d:%s\n", \
			__FILE__, __LINE__, __func__); \
	} while(0); \

// typedef struct CharList CharList;
// typedef struct CharList {
// 	char     data;
// 	size_t   index;
// 	CharList *next;
// } CharList;

typedef struct Line Line;

typedef struct Line {
	size_t  cap;
	size_t  size;
	char    *number_cstr;
	char    *content;
	size_t  number; // (y)
	size_t  col;    // (x)
	size_t  x_offset;
	Line    *next;
	Line    *prev;
} Line;

typedef struct cursor_s {
	int x, x_max, y, y_max, x_offset;
	char *x_cstr;
	char *y_cstr;
} Cursor;

typedef struct editor {
	FILE   *handle;
    Line   *first_line;
    Line   *last_line;
    Line   *line;
	Cursor *cursor;
	WINDOW *window;
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

// Line linked list
Line *line_new();
void line_connect(Line **end);
Line *line_next(Line *current);
Line *line_prev(Line *current);
void  lines_free(Line *line);


#endif  // MI_H
