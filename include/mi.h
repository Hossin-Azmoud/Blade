#ifndef MI_H
#define MI_H
#include <logger.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <clipboard.h>
#include <colors.h>

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
#define CTRL(x) ((x) & 037)
#define LINE_SZ 512
#define LINE_NUM_MAX 8
#define MAX_TOKENS 64
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

typedef enum ScriptType {
    PYTHON = 0,
    C,
    JS,
    UNSUP
} ScriptType;

typedef enum editorMode {
    NORMAL = 0,
    VISUAL,
    INSERT
} editorMode;

#define MAX_KEY_BINDIND 2

typedef enum bindingKind {
    COPY_LINE, // yy
    DEL_LINE,  // dd || cc
    NOT_VALID
} bindingKind;

// It is okay cuz it does not need to be dynamic..
typedef struct KeywordList {
    int size;
    char *extension;
    char *_list[100];
} KeywordList;

// TODO: IMPLEMET FUNCTIONS THAT CAN COLLECT THESE TOKENS..
typedef enum MITokenType {
    /* Groups */
    STR_LIT_INCOM,
    STR_LIT,
    NUMBER_LIT,
    KEYWORD,
    ID,
    /* Special tokens. */
    CALL,
    C_INCLUDE,
    C_DEFINE,
    COMMENT,
    /* Syms */
    EQ, // =
    GT, // >
    LT, // <
    AST, // *
    AND, // &
    PIPE, // |
    HASHTAG, // #
    OPAR_, // (
    CPAR_, // )
    OCERLY_, // {
    CCERLY_, // }
    OBRAC_, // [
    CBRAC_, // ]
    COMA, // ,
    PERIOD, // .
    DOLLARSIGN, // $
    AT, // @
    BANG, // !
    PLUS, // +
    MINUS, // -
    SEMICOLON, // ;
    COLON, // :
    FSLASH, // /
    BSLASH, // '\' /
    OTHER_PUNCT, // Any unknown punct.
} MITokenType;


typedef struct vKeyBindingQueue {
    char keys[MAX_KEY_BINDIND];
    int size;
    bindingKind kind;
} vKeyBindingQueue;

typedef struct MIToken {
    MITokenType kind;
    int xstart, xend;
    /* char data[512]; */
} MIToken;

typedef struct TokenList {
    MIToken *_list;
    int size, cap;
} TokenList;

typedef struct Chunk {
    char *data;
    int cap, size;
} Chunk;

typedef struct Line Line;

typedef struct Vec2 {
    int x, y;
    Line *_line;
} Vec2;

typedef struct Line {
    int     x, y, size, padding, cap, tokens_size;
    char    line_number[LINE_NUM_MAX];
    char    *content;
    Line    *next, *prev;
    TokenList token_list;
} Line;
    
typedef struct Lines_renderer {
    Line       *origin;
    Line       *start;
    Line       *end;
    Line       *current;
    int        win_h, win_w;
    int        max_padding;
    ScriptType script_type;
    int        count;
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
void editor_push_data_from_clip(Lines_renderer *line_ren);
char *editor_render_startup(int x, int y);
void lines_shift(Line *head, int num);


void editor_dl(Line *line);

void editor_apply_move(Lines_renderer *line_ren);

void editor_new_line(Lines_renderer *line_ren, bool reset_borders);
void free_lines(Line *lines);
void line_push_char(Line *line, char c, bool pasted);
void line_disconnect_from_ren(Lines_renderer *line_ren);

Line *Alloc_line_node(int row);

Result *make_prompt_buffer(int x, int y);
int    highlight_until_current_col(Vec2 start, Lines_renderer *line_ren);
void   editor_paste_content(Vec2 start, Vec2 end, Lines_renderer *line_ren);
void  clipboard_save_chunk(Vec2 start, Vec2 end);

// Editor movement stuff..
void editor_up(Lines_renderer *line_ren);
void editor_left(Lines_renderer *line_ren);
void editor_down(Lines_renderer *line_ren);
void editor_right(Lines_renderer *line_ren);
void handle_move(int c, Lines_renderer *line_ren);
bool is_move(int key);
void editor_details(Lines_renderer *line_ren, char *file_path, editorMode mode, char *notification);
void editor_handle_binding(Lines_renderer *line_ren, vKeyBindingQueue *bindings);
void editor_identify_binding(vKeyBindingQueue *bindings);
// CHUNK

void  chunk_append_s(Chunk *c, char *str);
void  chunk_append_char(Chunk *c, char chr);
Chunk *chunk_new();
void  chunk_distroy(Chunk *c);

// Token List ops
void token_list_append(TokenList *list, MITokenType kind, int xstart, int xend);
void retokenize_line(Line *line, ScriptType script_type);
KeywordList *get_keywords_list(ScriptType s);
bool is_keywrd(char *keywords[], char *word, int keywords_sz);
char *get_token_kind_s(MITokenType t);

ScriptType get_script_type(char *spath);
char *script_type_as_str(ScriptType s);
#endif // MI_H

