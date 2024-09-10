#include "parser.h"
#include <string.h>

char *memxcpy(char *src, int n)
{
  char *dst = malloc(n + 1);
  int i = 0;
  while (src[i] && n) {
    dst[i] = src[i];
    n--;
    i++;
  }
  dst[i] = 0;
  return (dst);
}

void token_list_append(char *line, TokenList *list, MITokenType kind, int xstart, int xend)
{
  if (list->size >= list->cap) {
    list->cap *= 2;
    list->_list = realloc(list->_list, sizeof(list->_list[0]) * list->cap);
  }     

  MIToken *current_tok = (list->_list + list->size);
  current_tok->xend = xend;
  current_tok->xstart = xstart;
  current_tok->kind = kind;
  current_tok->data = memxcpy(line + xstart, (xend - xstart) + 1);

  list->size++;
}

static int trim_spaces_left(char *buff, int curr) {
  int i = curr;
  while (isspace(buff[i]) && buff[i]) i++;
  return i;
}


static int collect_string_lit(TokenList *token_list, char *line, int current_x)
{
	int xstart = current_x++; 
	int xend;

	while (line[current_x]) {
		switch (line[current_x]) {
			case '\'':
			case '\"': {
				xend = current_x;
				current_x++;
				goto NEXT;
			} break;
			case '\\': {
				if (current_x + 1 < line[current_x])
					current_x = current_x + 2;
			} break;
			default: {
				current_x++;
			} break;
		}
	}

	xend = current_x;
	current_x++;
NEXT:
	if (line[xend] == line[xstart]) {
		token_list_append(line, token_list,
      STR_LIT, 
      xstart, 
      xend
    );
	} else {
		token_list_append(line, token_list, 
			STR_LIT_INCOM, 
			xstart, 
			xend);
	}

	return (current_x);
}

static int ishexdigit(char c) {
	return (
		(c >= 'A' && c <= 'F')
		||	
		(c >= 'a' && c <= 'f')
	);
}

static int collect_digit(TokenList *token_list, char *line, int current_x)
{
	int xstart = current_x++;
	int xend;

	
	while (isdigit(line[current_x]) || ((line[current_x] == 'x' || line[current_x] == 'X'))) {
		
		if (line[current_x] == 'x' || (line[current_x] == 'X' && (xstart - current_x == 1))) {
			// NOTE: (xstart - current_x == 1) means that x is the second char. so it could be a potential hex number.
			// TODO: Collect the hex digits.
			// skip the x/X
			current_x++;
			while (isdigit(line[current_x]) || ishexdigit(line[current_x])) {
				current_x++;
			}
			break;
		}

		current_x++;
	}

	xend = (current_x - 1);
	token_list_append(line, token_list, 
		NUMBER_LIT,
		xstart,
		xend
	);

	return current_x;
}

TokenList *retokenize_line(char *line)
{
	TokenList *token_list = malloc(sizeof(*token_list));
  // init the memory.
  {
    memset(token_list, 0, sizeof(*token_list));
    token_list->cap = 4;
    token_list->size = 0;
    token_list->_list = 
      malloc(sizeof(MIToken) * token_list->cap);
  }
  int xend = 0, xstart = 0, data_idx = 0, x = 0;

	for (; line[x];) {
    x = trim_spaces_left(line, x);
    xstart = x;

    // Collect a string lit.
    if (line[x] == '\"' || line[x] == '\'') {
      x = collect_string_lit(token_list, line, xstart);
      continue;
    }
    
    if (isdigit(line[x])) {
      x = collect_digit(token_list, line, xstart);
      continue;
    }

        // For ids, keywords and other syntaxes!
    if (isalpha(line[x]) || line[x] == '_') {
      while ((isalnum(line[x]) || (line[x] == '_')) && x < line[x]) {
        xend = (x);
        x++;
      }
      token_list_append(line, token_list, 
        ID,
        xstart,
        xend
      );
      continue;
    } 

        if (ispunct(line[x])) {
            switch (line[x]) {
                case '=': { 
                    // ID ID EQ
                    token_list_append(line, token_list, EQ, x, x);
                    x++;
                } break;
                       
                case '>': {
                    token_list_append(line, token_list, GT, x, x);
                    x++;
                } break;
                case '<': {
                    token_list_append(line, token_list, LT, x, x);
                    x++;
                } break;
                
                case '*': {

                    token_list_append(line, token_list, AST, x, x);
                    x++;
                } break;
                       
                case '&': {
                    token_list_append(line, token_list, AND, x, x);
                    x++;
                } break;
         
                case '|': {
                    token_list_append(line, token_list, PIPE, x, x);
                    x++;
                } break;
                       
                case '#': {
                    token_list_append(line, token_list, HASHTAG, x, x);
                    x++;
                } break;
                
                case OPAR: {
                    token_list_append(line, token_list, OPAR_, x, x);
                    x++;
                } break;
                
                case CPAR: {
                    token_list_append(line, token_list, CPAR_, x, x);
                    x++;
                } break;

                case OCERLY: {
                    token_list_append(line, token_list, OCERLY_, x, x);
                    x++;
                } break;
                
                case CCERLY: {
                    token_list_append(line, token_list, CCERLY_, x, x);
                    x++;
                } break;
                
                case OBRAC: {
                    token_list_append(line, token_list, OBRAC_, x, x);
                    x++;
                } break;
                
                case CBRAC: {
                    token_list_append(line, token_list, CBRAC_, x, x);
                    x++;
                } break;
                
                case ',': {
                    token_list_append(line, token_list, COMA, x, x);
                    x++;
                } break;
                       
                case '.': {
                    token_list_append(line, token_list, PERIOD, x, x);
                    x++;
                } break;
                
                case '$': {
                    token_list_append(line, token_list, DOLLARSIGN, x, x);
                    x++;
                } break;

                case '@': {
                    token_list_append(line, token_list, AT, x, x);
                    x++;
                } break;
                
                case '!': {
                    token_list_append(line, token_list, BANG, x, x);
                    x++;
                } break;
                
                case '+': {
                    token_list_append(line, token_list, PLUS, x, x);
                    x++;
                } break;
                   
                case '-': {
                    token_list_append(line, token_list, MINUS, x, x);
                    x++;
                } break;
                
                case ';': {
                    token_list_append(line, token_list, SEMICOLON, x, x);
                    x++;
                } break;
                case ':': {
                    token_list_append(line, token_list, COLON, x, x);
                    x++;
                } break;

                case '/': {
                    token_list_append(line, token_list, FSLASH, x, x);
                    x++;
                } break;
                
                case '\\': {
                    token_list_append(line, token_list, BSLASH, x, x);
                    x++;
                } break;
                default: {
                    token_list_append(line, token_list, OTHER_PUNCT, x, x);
                    x++;
                }
            }
        }
    }
  return (token_list);
}
