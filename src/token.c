#include <mi.h>
// TODO: Make a collector for string lits, number, id tokens.. and also keywords....
char *get_token_kind_s(MITokenType t)
{
    switch (t) {
        case STR_LIT: return "STR_LIT";
        case KEYWORD: return "KEYWORD";
        case NUMBER_LIT: return "NUMBER_LIT";
        case ID: return "ID";
        case STR_LIT_INCOM: return "STR_LIT_INCOM";
        case EQ: return "EQ";
        case GT: return "GT";
        case LT: return "LT";
        case AST: return "AST";
        case AND: return "AND";
        case PIPE: return "PIPE";
        case HASHTAG: return "HASHTAG";
        case OPAR_: return "OPAR_";
        case CPAR_: return "CPAR_";
        case OCERLY_: return "OCERLY_";
        case CCERLY_: return "CCERLY_";
        case OBRAC_: return "OBRAC_";
        case CBRAC_: return "CBRAC_";
        case COMA: return "COMA";
        case PERIOD: return "PERIOD";
        case DOLLARSIGN: return "DOLLARSIGN";
        case AT: return "AT";
        case BANG: return "BANG";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case FSLASH: return "FSLASH";
        case BSLASH: return "BSLASH";
        case OTHER_PUNCT: return "OTHER_PUNCT";
        default: return "UNKNOWN_TOKEN_KIND";
    } 
}

KeywordList *get_keywords_list(char *ext)
{
    static KeywordList list[KEYWORDLIST_SZ] = {
        [0] = {
            .extension="py", 
            .size=18,
            ._list={
                "def", 
                "for", 
                "if", 
                "while", 
                "else", 
                "elif", 
                "True", 
                "False", 
                "max",
                "min",
                "match",
                "class",
                "str",
                "range",
                "in",
                "while",
                "import",
                "from",
                "lambda"
            },
        }
    };

    for (int it = 0; it < KEYWORDLIST_SZ; ++it) {
        if (strcmp(ext, list[it].extension) == 0) {
            return (list + it);
        }
    }

    return NULL;
}

void token_list_append(TokenList *list, MITokenType kind, int xstart, int xend)
{
    if (list->size >= list->cap) {
        list->cap *= 2;
        list->_list = realloc(list->_list, sizeof(list->_list[0]) * list->cap);
    }     

    MIToken *current_tok = (list->_list + list->size);
    current_tok->xend = xend;
    current_tok->xstart = xstart;
    current_tok->kind = kind;
    list->size++;
}
static int trim_spaces_left(char *buff, int curr) {
    int i = curr;
    while (isspace(buff[i]) && buff[i]) i++;
    return i;
}

void retokenize_line(Line *line, KeywordList *keywords_list)
{
    char temp[512] = { 0 };
    int xend = 0, xstart = 0, data_idx = 0, x = 0;
    // char c = 0;
    
    if (line->token_list.size > 0) {
        // Reinit the tokens.
        memset(line->token_list._list, 0, sizeof(line->token_list._list[0]) * line->token_list.cap);
        line->token_list.size = 0;
    }

    for (; x < line->size;) {
        x = trim_spaces_left(line->content, x);
        xstart = x;

        // Collect a string lit.
        if (line->content[x] == '\"' || line->content[x] == '\'') {
            xstart = x;
            temp[data_idx++] = line->content[x++];

            while (x < line->size) {
                switch (line->content[x]) {
                    case '\'':
                    case '\"': {
                        if (line->content[x] == line->content[xstart]) {
                            xend = x;
                            temp[data_idx] = line->content[x];
                            token_list_append(&(line->token_list), STR_LIT, xstart, xend);
                            data_idx = 0;
                            memset(temp, 0,  512);
                            temp[data_idx++] = line->content[x++];
                            goto next;        
                        }
                        temp[data_idx++] = line->content[x++];
                    } break;
                    case '\\': {
                        if (x + 1 < line->size) {
                            temp[data_idx++] = line->content[x++]; 
                            temp[data_idx++] = line->content[x++];
                        }
                    } break;
                    default: {
                        temp[data_idx++] = line->content[x++];
                    } break;
                }
            }

            next:
            // Incomplete string literal
            if (x == line->size && line->content[xend] != line->content[xstart]) {
                xend = x - 1;    
                token_list_append(&(line->token_list), 
                    STR_LIT_INCOM, 
                    xstart, 
                    xend);
            }

            continue;
        }
        
        // isalnum,  isalpha, isascii, isblank, iscntrl, isdigit, isgraph, islower, isprint, ispunct, isspace, isupper, isxdigit, isalnum_l, isalpha_l, isas‐cii_l, isblank_l, iscntrl_l, isdigit_l, isgraph_l, islower_l, isprint_l, ispunct_l, isspace_l, isupper_l, isxdigit_l  -  character  classification
        // Digit collector
        if (isdigit(line->content[x])) {
            xstart = x;
            temp[data_idx++] = line->content[x++];

            while (isdigit(line->content[x]) && x < line->size) {
                temp[data_idx++] = line->content[x++];
                xend = x - 1;
            }

            token_list_append(&(line->token_list), 
                    NUMBER_LIT,
                    xstart,
                    xend);
            data_idx = 0;
            memset(temp, 0,  512);
            continue;
        }

        // For ids, keywords and other syntaxes!
        if (isalpha(line->content[x]) || line->content[x] == '_') {
            xstart = x;
            temp[data_idx++] = line->content[x++];

            while ((isalnum(line->content[x]) || (line->content[x] == '_')) && x < line->size) {
                temp[data_idx++] = line->content[x];
                xend = x;
                x++;
            }

            fprintf(get_logger_file_ptr(), "ADDDING : %s\n", temp);
            token_list_append(&(line->token_list), 
                (is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : ID,
                xstart,
                xend);
            data_idx = 0;
            memset(temp, 0,  512);
            continue;
        } 

        if (ispunct(line->content[x])) {
            switch (line->content[x]) {
                case '=': {
                    token_list_append(&(line->token_list), EQ, x, x);
                    x++;
                } break;
                       
                case '>': {
                    token_list_append(&(line->token_list), GT, x, x);
                    x++;
                } break;
                case '<': {
                    token_list_append(&(line->token_list), LT, x, x);
                    x++;
                } break;
                
                case '*': {
                    token_list_append(&(line->token_list), AST, x, x);
                    x++;
                } break;
                       
                case '&': {
                    token_list_append(&(line->token_list), AND, x, x);
                    x++;
                } break;
         
                case '|': {
                    token_list_append(&(line->token_list), PIPE, x, x);
                    x++;
                } break;
                       
                case '#': {
                    token_list_append(&(line->token_list), HASHTAG, x, x);
                    x++;
                } break;
                
                case OPAR: {
                    token_list_append(&(line->token_list), OPAR_, x, x);
                    x++;
                } break;
                
                case CPAR: {
                    token_list_append(&(line->token_list), CPAR_, x, x);
                    x++;
                } break;

                case OCERLY: {
                    token_list_append(&(line->token_list), OCERLY_, x, x);
                    x++;
                } break;
                
                case CCERLY: {
                    token_list_append(&(line->token_list), CCERLY_, x, x);
                    x++;
                } break;
                
                case OBRAC: {
                    token_list_append(&(line->token_list), OBRAC_, x, x);
                    x++;
                } break;
                
                case CBRAC: {
                    token_list_append(&(line->token_list), CBRAC_, x, x);
                    x++;
                } break;
                
                case ',': {
                    token_list_append(&(line->token_list), COMA, x, x);
                    x++;
                } break;
                       
                case '.': {
                    token_list_append(&(line->token_list), PERIOD, x, x);
                    x++;
                } break;
                
                case '$': {
                    token_list_append(&(line->token_list), DOLLARSIGN, x, x);
                    x++;
                } break;

                case '@': {
                    token_list_append(&(line->token_list), AT, x, x);
                    x++;
                } break;
                
                case '!': {
                    token_list_append(&(line->token_list), BANG, x, x);
                    x++;
                } break;
                
                case '+': {
                    token_list_append(&(line->token_list), PLUS, x, x);
                    x++;
                } break;
                   
                case '-': {
                    token_list_append(&(line->token_list), MINUS, x, x);
                    x++;
                } break;
                
                case ';': {
                    token_list_append(&(line->token_list), SEMICOLON, x, x);
                    x++;
                } break;
                case ':': {
                    token_list_append(&(line->token_list), COLON, x, x);
                    x++;
                } break;

                case '/': {
                    token_list_append(&(line->token_list), FSLASH, x, x);
                    x++;
                } break;
                
                case '\\': {
                    token_list_append(&(line->token_list), BSLASH, x, x);
                    x++;
                } break;
                default: {
                    token_list_append(&(line->token_list), OTHER_PUNCT, x, x);
                    x++;
                }
            }
        }
    }
}

bool is_keywrd(char *keywords[], char *word, int keywords_sz) {

    for (int it = 0; it < keywords_sz; ++it) {
        if (strcmp(keywords[it], word) == 0) {
            return true;
        }
    }

    return false;
}
