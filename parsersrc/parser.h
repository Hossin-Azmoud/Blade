#ifndef PARSING_H
#define PARSING_H
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#define BASE_16_ASCII     "0123456789abcdef"
#define BASE_16_ASCII_UP  "0123456789abcdef"
#define BASE_10_ASCII     "0123456789"
#define BASE_BIN_ASCII    "01"

#define BASE_16_PREFIX      "0X"
#define BASE_16_PREFIX_UP   "0x"
#define BASE_BIN_PREFIX     "0b"

#define TAB         '\t'
#define NL          '\n'

#define OPAR        '('
#define CPAR        ')'
#define OCERLY      '{'
#define CCERLY      '}'

#define OBRAC       '['
#define CBRAC       ']'

#define DQUOTE      '\"'
#define SQUOTE      '\''
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
    C_TAG,
    C_INCLUDE_FILE,
    C_ENUM,
    C_TYPEDEF,
    C_STRUCT,
    C_UNION,
    TYPE,
    _GENERIC_NULL,
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
typedef struct MIToken {
    MITokenType kind;
    int xstart, xend;
    char *data;
} MIToken;

typedef struct TokenList {
    MIToken *_list;
    int size, cap;
} TokenList;
typedef struct MiStatus_s {
  bool ok;
  char *errmsg; // Should be exclusively static.. read only.
} MiStatus_t;

typedef struct EditorConfig_s {
  MiStatus_t status;
  bool autosave;
  char indent_char;
  int indent_count;
  int background;
  int foreground;
} EditorConfig_t;

typedef enum FileType {
    PYTHON = 0,
    C,
    JS,
    GO, 
    CNFG,
    MP3,
    UNSUP
} FileType;

long np_atoi_base(char *a, char *base);
EditorConfig_t *load_editor_config(char *file);
char	**split(char *str, char *charset, size_t *count);
char *read_next_line(int fd);
TokenList *retokenize_line(char *data);
void token_list_append(char *line, TokenList *list, MITokenType kind, int xstart, int xend);
#endif // PARSING_H
