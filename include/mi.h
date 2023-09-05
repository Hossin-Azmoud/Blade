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


typedef struct Line Line;

typedef struct Line {
	size_t  cap;
	size_t  size;
	char    *content;
	size_t  number;
	size_t  x_offset;
	Line    *next;
	Line    *prev;
} Line;

typedef struct cursor_s {
	int x, x_max, y, y_max, x_offset;
	char *x_cstr;
	char *y_cstr;
} Cursor;

typedef struct mifile {
	FILE   *handle;
	Line   *lines_start;
	Line   *lines_end;
	Line   *lines;
} MiFile;

typedef struct editor {
	MiFile file;
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
Line *line_construct_new(size_t number);
void lines_free(Line *line);

#endif  // MI_H
