#ifndef MI_H
#define MI_H
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// color editor pairs
#define SECONDARY_THEME_PAIR 1
#define MAIN_THEME_PAIR 2
#define ERROR_PAIR      3

#define CTRL(x) ((x) & 0x1f)
#define LINE_SZ 256
#define LINE_NUM_MAX 8
#define TAB '\t'
#define NL  '\n'

#define OPAR '('
#define CPAR ')'
#define OCERLY '{'
#define CCERLY '}'

#define OBRAC '['
#define CBRAC ']'

#define DQUOTE '\"'
#define SQUOTE '\''
#define WLCM_BUFF "(WELLCOME TO MI EDITOR V0.1!)"

typedef struct Line Line;

typedef struct Line {
    int x, y, size, padding;
    char line_number[LINE_NUM_MAX];
    char content[LINE_SZ];
    Line *next, *prev;
} Line;
    
typedef struct Lines_renderer {
    Line   *origin;
    Line   *start;
    Line   *end;
    Line   *current;
    int    win_h, win_w;
    int    max_padding;
    int    count;
} Lines_renderer;

WINDOW *init_editor();
int load_file(char *file_path, Lines_renderer *line_ren);
void save_file(char *file_path, Line *lines);
void render_lines(Lines_renderer *line_ren);
void editor_tabs(Line *line);
void editor_backspace(Lines_renderer *line_ren);
char *editor_render_startup(int x, int y);


void editor_up(Lines_renderer *line_ren);
void editor_left(Lines_renderer *line_ren);
void editor_down(Lines_renderer *line_ren);
void editor_right(Lines_renderer *line_ren);

void editor_apply_move(Lines_renderer *line_ren);

void editor_new_line(Lines_renderer *line_ren);
void free_lines(Line *lines);
void line_push_char(Line *line, char c);
void editor_details(Lines_renderer *line_ren, char *file_path);
Line *Alloc_line_node(int row);

#endif // MI_H
