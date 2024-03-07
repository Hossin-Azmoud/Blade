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
        case CALL: return "CALL";
        case COMMENT: return "COMMENT";
        default: return "UNKNOWN_TOKEN_KIND";
    } 
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

void retokenize_line(Line *line, ScriptType script_type)
{
    char temp[512] = { 0 };
    int xend = 0, xstart = 0, data_idx = 0, x = 0;
    KeywordList *keywords_list = get_keywords_list(script_type);     

    if (line->token_list.size > 0) {
        // Reinit the tokens.
        line->token_list._list = memset(line->token_list._list, 0, sizeof(line->token_list._list[0]) * line->token_list.cap);
        line->token_list.size = 0;
    }

    for (; x < line->size;) {
        x = trim_spaces_left(line->content, x);
        xstart = x;

        // Collect a string lit.
        if (line->content[x] == '\"' || line->content[x] == '\'') {
            x++;
            while (x < line->size) {
                switch (line->content[x]) {
                    case '\'':
                    case '\"': {
                        xend = x;
                        x++;
                        goto next;
                    } break;
                    case '\\': {
                        if (x + 1 < line->size)
                            x = x + 2;
                    } break;
                    default: {
                        x++;
                    } break;
                }
            }

            xend = x;
            x++;
            next:
            if (line->content[xend] == line->content[xstart]) {
                token_list_append(&(line->token_list), 
                                  STR_LIT, 
                                  xstart, 
                                  xend);
            } else {
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
            x++;

            while (isdigit(line->content[x]) && x < line->size) {
                xend = (x);
                x++;
            }
            
            token_list_append(&(line->token_list), 
                NUMBER_LIT,
                xstart,
                xend
            );
            continue;
        }

        // For ids, keywords and other syntaxes!
        if (isalpha(line->content[x]) || line->content[x] == '_') {
            temp[data_idx++] = line->content[x++];
            while ((isalnum(line->content[x]) || (line->content[x] == '_')) && x < line->size) {
                xend = (x);
                temp[data_idx++] = line->content[x++];
            }

            if (line->token_list.size > 0 && script_type == C) {
                if (line->token_list._list[line->token_list.size - 1].kind == HASHTAG) {
                    // TODO: Get the word and see the type of the c_tag is it an #include or a condition #if #endif #ifndef,
                    if (!strcmp(temp, "include")) {
                        line->token_list._list[line->token_list.size - 1].kind = C_INCLUDE;
                    } else {
                        line->token_list._list[line->token_list.size - 1].kind = C_TAG;
                    }
    
                    line->token_list._list[line->token_list.size - 1].xend = xend;
                    data_idx = 0;
                    memset(temp, 0,  512);
                    continue;
                }                
            }

            token_list_append(&(line->token_list), 
                (is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : ID,
                xstart,
                xend
            );

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
                    if (script_type == C) { // gather an include source.
                        xstart = x;

                        while (x < line->size && line->content[x] != '>') {
                            x++;
                        }

                        if (line->content[x] == '>') {
                            token_list_append(&(line->token_list), C_INCLUDE_FILE, xstart, x);
                            x++;
                            continue;
                        }
                        x = xstart;
                    }

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
                    if (script_type == PYTHON) {
                        token_list_append(&(line->token_list), COMMENT, x, line->size);
                        return;
                    }
                    
                    token_list_append(&(line->token_list), HASHTAG, x, x);
                    x++;
                } break;
                
                case OPAR: {
                    if (line->token_list.size > 0) {
                        if (line->token_list._list[line->token_list.size - 1].kind == ID) {
                            line->token_list._list[line->token_list.size - 1].kind = CALL;
                        }
                    }

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
                    if ((x + 1 < line->size)) {
                        if (script_type == C && line->content[x + 1] == '/') {
                            // WE Collect a comment.
                            token_list_append(&(line->token_list), COMMENT, x, line->size);
                            return;
                        }
                    }
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

