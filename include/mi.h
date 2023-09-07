#ifndef MI_H
#define MI_H

#include <stdlib.h>
#include <assert.h>
#include <curses.h>
#include <string.h>

#define MAX_DIG    32
#define NL         '\n'
#define DEFUALT_LINE_CAP 1024
#define LOG \
	do { \
		printf("[LOGGER] %s:%d:%s\n", \
			__FILE__, __LINE__, __func__); \
	} while(0); \


typedef struct Line Line;
typedef struct CharList CharList;

typedef struct CharList {
	char data;
	CharList *next;
	CharList *prev;
} CharList;


typedef struct cursor_s {
	size_t  row;  // (y)
	size_t  col_offset;
	size_t  col;  //(x)
	char    *row_cstr;
} Cursor;

typedef struct Line {
	Cursor  cursor;
	size_t  cap;
	size_t  size;
	CharList *data;
	CharList *last_col;
	CharList *first_col;
	char  *content; // TODO: MAKE THIS A LINKED LIST!
	Line  *next;
	Line  *prev;
} Line;

typedef struct editor {
	FILE   *handle;
    Line   *first_line;
    Line   *last_line;
    Line   *line;
	WINDOW *window;
} MiEditor;

// EDITOR
MiEditor *editor_new(char *file);
void editor_distroy(MiEditor *E);
void editor_write_line_number(MiEditor *E);
MiEditor *editor_init(char *file);
void editor_new_line(MiEditor *E);
void editor_render_char(MiEditor *E, char c);
void editor_push_char(MiEditor *E, char c);
void editor_back_space(MiEditor *E);
void editor_pop_char(MiEditor *E);
// STD
size_t digit_len(size_t n);

// Line linked list
Line *line_new();
void line_connect(Line **end);
Line *line_next(Line *current);
Line *line_prev(Line *current);
void  lines_free(Line *line);

// CharacterList
CharList *charlist_new();
void charlist_free(CharList *head);

#endif  // MI_H
