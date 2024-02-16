#include <mi.h>

void editor_up(Lines_renderer *line_ren)
{
    if (line_ren->current->prev) {
        if (line_ren->current->y - line_ren->start->y == 0) {
            line_ren->start = line_ren->start->prev;
            if (line_ren->end->prev) {
                line_ren->end = line_ren->end->prev;
            }

        }
        line_ren->current = line_ren->current->prev;
    }
}

void editor_left(Lines_renderer *line_ren)
{
    if (line_ren->current->x > 0) line_ren->current->x--;
}

void editor_down(Lines_renderer *line_ren)
{
    if (line_ren->current->next){
        if (line_ren->current->y - line_ren->start->y == line_ren->win_h - 4) {
            line_ren->start = line_ren->start->next;
            if (line_ren->end->next) {
                line_ren->end = line_ren->end->next;
            }
        }

        line_ren->current = line_ren->current->next;
    }
}
void editor_right(Lines_renderer *line_ren)
{
    if (line_ren->current->x < line_ren->current->size) line_ren->current->x++;
}
