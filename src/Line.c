#include <mi.h>

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
