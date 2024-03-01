#include <mi.h>

char *get_token_kind_s(MITokenType t)
{
    switch (t) {
    
    case STR: { 
        return "STR"; 
    } break;
    case KEYWORD: { 
        return "KEYWORD"; 
    } break;
    default: return "UNKNOWN_TOKEN_KIND";
    } 
}

KeywordList *get_keywords_list(char *ext)
{
    static KeywordList list[KEYWORDLIST_SZ] = {
        [0] = {
            .extension="py", 
            .size=13,
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
                "str"
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
        list->_list = realloc(list->_list, list->cap);
    }     
    MIToken *current_tok = (list->_list + list->size);
    current_tok->xend = xend;
    current_tok->xstart = xstart;
    current_tok->kind = kind;
    list->size++;
}

void tokenize_line(Line *line, KeywordList *keywords_list)
{
    char temp[512] = { 0 };
    int xend = 0, xstart = 0, data_idx = 0, x = 0;

    while (isspace(line->content[x])) x++;
    xstart = x;
    
    for (; x < line->size; ++x, data_idx++) {

        if (x == line->size - 1) {
            xend = x;
            temp[data_idx] = line->content[x];
            token_list_append(&(line->token_list), (is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : STR, xstart, xend);
            break;
        };

        if (isspace(line->content[x])) { 
            xend = x - 1;
            token_list_append(&(line->token_list), (is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : STR, xstart, xend);
            while (isspace(line->content[x])) x++;
            xstart = x;
            data_idx = 0;
            memset(temp, 0,  512);
            temp[data_idx] = line->content[x];
            continue;
        }

        // Not a space and not the latest.
        temp[data_idx] = line->content[x];
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
