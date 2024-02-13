#ifndef MI_H
#define MI_H
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define LINE_SZ 256
#define LINE_NUM_MAX 4
#define TAB '\t'
typedef struct Line Line;

typedef struct Line {
    size_t x, y, size;
    char content[LINE_SZ];
    Line *next, *prev;
} Line;

WINDOW *init_editor(char *file_name);
size_t load_file(char *file_path, Line *lines);
void save_file(char *file_path, Line *lines, size_t save_count);
void render_lines(Line *lines);
void editor_tabs(Line *line);
void line_push_char(Line *line, char c);
void debugger(WINDOW *win, size_t x, size_t y, size_t size);
Line *Alloc_line_node(size_t row);

#endif // MI_H
