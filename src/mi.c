#include <mi.h>
// int fgetc(FILE *stream);*
Line *Alloc_line_node(int row)
{
    Line *line = (Line *)malloc(sizeof(Line));

    line->next = NULL;
    line->prev = NULL;
    line->x    = 0;
    line->size = 0;
    line->y    = row;

    for (int i = 0; i < LINE_SZ; ++i)
        line->content[i] = 0x0;

    return (line);
}

WINDOW *init_editor()
{
    WINDOW *win = initscr();

    raw();
	keypad(stdscr, TRUE);
	noecho();
    cbreak();
    start_color();
    // pair_number, foreground, background
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    wbkgd(win, COLOR_PAIR(1));

    return win;
}
int load_file(char *file_path, Lines_renderer *line_ren)
{
    Line *new = NULL;
    char c;
    FILE *Stream = fopen(file_path, "r");

    if (!Stream) {
        return 0;
    }

    while((c = fgetc(Stream)) != EOF) {
        if (c == '\n') {
            new = Alloc_line_node(line_ren->current->y + 1);
            line_ren->current->next = new;
            new->prev = line_ren->current;
            line_ren->current = new;

            if (line_ren->current->y - line_ren->start->y == line_ren->win_h - 4) { 
                // when we reach the last row that will be rendered we nark i.
                line_ren->end = line_ren->current;
            }

            line_ren->count++;
        } else {
            line_ren->current->content[line_ren->current->x++] = c;
            line_ren->current->size++;
        }

        if (!line_ren->count) line_ren->count++;
    }
    
    if (line_ren->count > 1 && (line_ren->start == line_ren->end)) {
        line_ren->end = line_ren->current;
    }

    line_ren->current = line_ren->start;
    fclose(Stream);
    return line_ren->count;
}

void save_file(char *file_path, Line *lines, int save_count) {
    FILE *Stream = fopen(file_path, "w+");
    Line *next = NULL;
    Line *current = lines;

    for (int i = 0; (i < save_count && current); ++i) {
        fprintf(Stream, "%s\n", current->content);
        next = current->next;
        free(current);
        current = next;
    }

    fclose(Stream);
}

void free_lines(Line *lines) {
    Line *next = NULL;
    Line *current = lines;

    for (; (current);) {
        next = current->next;
        free(current);
        current = next;
    }
}

static void render_line(Line *line, int offset)
{
    int x = 0, n = 0;
    char line_number[LINE_NUM_MAX] = { 0 };
    line->padding = sprintf(line_number, "%u  ", line->y + 1);

    for (n = 0; n < line->padding; n++) {
        mvaddch(line->y - offset, n, line_number[n]);
    }
        
    for (x = 0; x < line->size; ++x) {
         mvaddch(line->y - offset, x + line->padding, line->content[x]);
    }
}

void editor_backspace(Lines_renderer *line_ren)
{

    if (line_ren->current->x > 0) {
        memmove(
            line_ren->current->content + line_ren->current->x - 1,
            line_ren->current->content + line_ren->current->x,
            line_ren->current->size - line_ren->current->x
        );
   
        line_ren->current->x--;
        line_ren->current->size--;
        line_ren->current->content[line_ren->current->size] = 0;
        return;
    }

    if (line_ren->current->y > 0) {
        line_ren->current = line_ren->current->prev; // go back one line.
        line_ren->current->content[line_ren->current->x] = 0;
        if (line_ren->current->x) {
            line_ren->current->x--;
            line_ren->current->size--;
        }
        return;
    }

    line_ren->current->content[line_ren->current->x] = 0;
}
void render_lines(Lines_renderer *line_ren)
{
    Line *current = line_ren->start;
    while (current) {
        render_line(current, line_ren->start->y);
        if (current == line_ren->end) break;
        current = current->next;
    }
}

void editor_details(Lines_renderer *line_ren, char *file_path)
{
    mvprintw(line_ren->win_h - 2, line_ren->win_w - (60), "w=%d h=%d x -> %d y -> %d s -> %d start: %s end: %s", 
             line_ren->win_w,
             line_ren->win_h,
             line_ren->current->x, 
             line_ren->current->y, 
             line_ren->current->size,
             (line_ren->current == line_ren->start) ? "True" : "False",
             (line_ren->current == line_ren->end) ? "True" : "False"
    );
 
    mvprintw(line_ren->win_h - 2, 0, "Editing %s", file_path);
    for (int i = 0; i < line_ren->win_w; ++i) {
        mvprintw(line_ren->win_h - 3, i, "_");
    }

    move(line_ren->current->y, line_ren->current->x);
}

void line_push_char(Line *line, char c)
{
    memmove((line->content + line->x + 1),
            (line->content + line->x),
            line->size - line->x);
    line->content[line->x++] = c;
    line->size++;
}

void editor_tabs(Line *line)
{
    line_push_char(line, TAB);
}

void editor_new_line(Lines_renderer *line_ren)
{
    Line *new, *next, *prev;

    new = Alloc_line_node(line_ren->current->y + 1); 
    if (!(line_ren->current->next) && line_ren->current->x == line_ren->current->size) 
    {
        line_ren->current->next = new;
        new->prev = line_ren->current;
        line_ren->end = new;
        line_ren->current = new;
        
        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) {
            line_ren->start = line_ren->start->next;
        }

        line_ren->count++;
        return;
    }

    if (line_ren->current->next && line_ren->current->x == line_ren->current->size) {
        next = line_ren->current->next;
        new->prev = line_ren->current; // correct
        new->next = next; // correct.
        line_ren->current->next = new; // correct
        next->prev = new;
        new = new->next;

        while(new) {
            new->y++;
            new = new->next;
        }

        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) 
            line_ren->end = line_ren->end->prev;

        line_ren->current = line_ren->current->next;
        return;
    }
    
    // shift and add a line.
    prev = line_ren->current->prev;
    new = Alloc_line_node(line_ren->current->y);
    new->next = line_ren->current;

    if (prev) {
        prev->next = new;
        new->prev = prev;
    }

    line_ren->current->prev = new;
    new = line_ren->current;
    
    while (new) {
        new->y++;
        new = new->next;
    }

    if (line_ren->current->y == 1) {
        line_ren->origin = line_ren->current->prev;
        line_ren->start  = line_ren->current->prev;
    }

    if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) {
        line_ren->start = line_ren->start->next;
        if (line_ren->end->next) 
            line_ren->end   = line_ren->end->next;
        else
            line_ren->end = line_ren->current;
    }

    // line_ren->current = line_ren->current->prev;
    line_ren->count++;

}
