#include <mi.h>

void indent_line(Line *line) 
{
    
    int x = line->x;
    line->x = 0;
    editor_tabs(line);
    line->x += x;
}

void unindent_line(Line *line) 
{
    int x = line->x;
    line->x = 0;

    while (line->content[line->x] == ' ' && line->x < 4) {
        line->x++;
    }

    memmove(line->content, 
            line->content + line->x, 
            line->size - line->x);

    line->size -= line->x;
    line->x = (x - line->x);
}

Line *Alloc_line_node(int row)
{
    Line *line = (Line *)malloc(sizeof(Line));

    line->next = NULL;
    line->prev = NULL;
    line->x    = 0;
    line->size = 0;
    line->cap  = LINE_SZ;
    line->y    = row;
    line->content = malloc(LINE_SZ);
    memset(line->content, 0x0, LINE_SZ);
    
    line->token_list = (TokenList){0}; 
    (line->token_list)._list = malloc(sizeof (MIToken) * MAX_TOKENS);
    memset(line->token_list._list, 0x0, sizeof (MIToken) * MAX_TOKENS);
    (line->token_list).cap = MAX_TOKENS;
    return (line);
}

void lines_shift(Line *head, int num) {

    Line *curr = head;
    for (;curr;) {
        curr->y += num;
        curr = curr->next;
    }
}


void free_lines(Line *lines) {
    Line *next = NULL;
    Line *current = lines;
    
    for (; (current);) {
        next = current->next;
        free(current->content);
        free((current->token_list)._list);
        free(current);
        current = next;
    }
}

Line *disconnect_line(Line *head)
{
    Line *current = head;
    Line *prev    = current->prev;
    Line *next    = current->next;
    
    if (!prev && !next) {
        return current;
    }

    if (prev)
        prev->next = next;

    if (next)
        next->prev = prev;

    free(current->content);
    free(current);
    lines_shift(next, -1); 
    return prev;
}


void char_inject(Line *line, char c)
{
    if (isprintable (c)) {
        memmove((line->content + line->x + 1),
            (line->content + line->x),
            line->size - line->x);
        line->content[line->x++] = c;
        line->size++;
    }
}

void line_push_char(Line *line, char c, bool pasted)
{
    if (line->size + 1 == line->cap) {
        line->cap *= 2;
        line->content = realloc(line->content, line->cap);
    }

    char_inject(line, c);
    if (pasted)
        return;

    switch (c) {
        case OPAR: {
            char_inject(line, CPAR);
            line->x--;
        } break;
        case OCERLY: {
            char_inject(line, CCERLY);
            line->x--;
        } break;
        case OBRAC: {
            char_inject(line, CBRAC);
            line->x--;
        } break;
        case DQUOTE: {
            char_inject(line, DQUOTE);
            line->x--;
        } break;
        case SQUOTE: {
            char_inject(line, SQUOTE);
            line->x--;
        } break;
        default: { } break;
    }
}

void editor_tabs(Line *line)
{
    for (int i = 0; i < 4; ++i)
        line_push_char(line, ' ', true);
}

