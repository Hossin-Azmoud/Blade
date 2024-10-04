#include "parser.h"
#include <assert.h>
// #include <cstddef>
#include <fcntl.h>
// #include <math.h>
#include <stdio.h>
#include <string.h>

void array_free(char **toks) {
  if (toks) {
    for (int v = 0; toks[v]; v++)
      free(toks[v]);
    free(toks);
  }
}
char *escape(char *src) {
  if (src == NULL)
    return NULL;
  char *bp;
  char *dst = malloc(sizeof(src));
  if (dst == NULL)
    return dst;
  bp = dst;
  for (;*src != 0x0;src++, dst++) {
    
    if (*src == '\\') {
      if (*(src + 1)) {
        switch (*(1+src)) {
          case 't': {
            *dst = TAB;
            src++;
          } break;
        }
        continue;
      }
    }
    *dst = *src;
  }
  return bp;
}


EditorConfig_t *load_editor_config(char *file) {
  EditorConfig_t *cfg;
  char **toks;
  int stream;
  int i = 0;
  if (!file)
    return NULL;
  cfg = malloc(sizeof(*cfg));
  memset(cfg, 0, sizeof(*cfg));
  stream = open(file, O_RDONLY);
  if (stream < 0) {
    free(cfg);
    return (NULL);
  }
  for (char *next = read_next_line(stream), i = 0; next;
       (next = read_next_line(stream)), i++) {
    TokenList *tokens = retokenize_line(next);
    int j = 0;
    for (int j = 0; j < tokens->size; j++)
      printf("LINE[%i] token->data[%i]: `%s`\n", i, j, (tokens->_list[j].data == NULL) ? "nil" : escape(tokens->_list[j].data));
  }
  // TODO: split it by : to get a key and a value.
  // free the allocated things.
  close(stream);
  array_free(toks);
  return (cfg);
}
