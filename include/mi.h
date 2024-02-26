#ifndef MI_H
#define MI_H
#include <logger.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ESC         0x1b
#define KEY_COPY_   'y'
#define KEY_PASTE_  'p'
#define KEY_INSERT_ 'i'
#define KEY_VISUAL_ 'v'
#define KEY_SAVE_   'w'
#define KEY_DEL     0x014a
#define MENU_HEIGHT_ 1
#define L_SHIFT 0x189
#define R_SHIFT 0x192


// color editor pairs
#define SECONDARY_THEME_PAIR 1
#define MAIN_THEME_PAIR      2
#define ERROR_PAIR           3
#define BLUE_PAIR            4
#define HIGHLIGHT_THEME      5 

#define CTRL(x) ((x) & 037)
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

typedef enum charType {
    NUMBER,
    SYMBOL,
    ALPHABET,
    SPACE,
    UNK
} charType;

charType get_class(int key);

typedef enum ResultType {
    SUCCESS,
    ERROR
} ResultType;

typedef enum ErrorType {
    NONE,
    EXIT_SIG,
    EMPTY_BUFF
} ErrorType;

typedef enum editorMode {
    NORMAL = 0,
    VISUAL,
    INSERT
} editorMode;


typedef struct Line Line;

typedef struct Vec2 {
    int x, y;
    Line *_line;
} Vec2;

// typedef struct Vcursor {
//     Vec2 start; // start position in which we will start the coppying!
//     Vec2 end; // end pos in which the coppying ends!
// } Vcursor;
//
typedef struct Line {
    int x, y, size, padding, cap;
    char line_number[LINE_NUM_MAX];
    char *content;
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

typedef struct Result {
    ResultType type;
    ErrorType  etype;
    char *data;
} Result;

WINDOW *init_editor();
int load_file(char *file_path, Lines_renderer *line_ren);
int save_file(char *file_path, Line *lines, bool release);
void render_lines(Lines_renderer *line_ren);
void editor_tabs(Line *line);
void editor_backspace(Lines_renderer *line_ren);
char *editor_render_startup(int x, int y);


void editor_dl(Line *line);

void editor_apply_move(Lines_renderer *line_ren);

void editor_new_line(Lines_renderer *line_ren, bool reset_borders);
void free_lines(Line *lines);
void line_push_char(Line *line, char c, bool pasted);
void editor_details(Lines_renderer *line_ren, char *file_path, editorMode mode, char *notification);
Line *Alloc_line_node(int row);

Result *make_prompt_buffer(int x, int y);
int    highlight_until_current_col(Vec2 start, Lines_renderer *line_ren);
void   editor_paste_content(Vec2 start, Vec2 end, Lines_renderer *line_ren);


// Editor movement stuff..
void editor_up(Lines_renderer *line_ren);
void editor_left(Lines_renderer *line_ren);
void editor_down(Lines_renderer *line_ren);
void editor_right(Lines_renderer *line_ren);
void handle_move(int c, Lines_renderer *line_ren);
bool is_move(int key);

#endif // MI_H

