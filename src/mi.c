
#include <mi.h>
#include <logger.h>
// Possible modes in the editor!
static char *modes[] = { 
    "NORMAL",
    "VISUAL",
    "INSERT",
    "FILEBROWSER"
};

WINDOW *init_ncurses_window()
{
    WINDOW *win = initscr();

    raw();
	keypad(stdscr, TRUE);
	noecho();
    cbreak();
    init_colors();

    wbkgd(win, COLOR_PAIR(MAIN_THEME_PAIR));
    set_escdelay(0); 
    {
        // Mouse stuff.
        mousemask(ALL_MOUSE_EVENTS, NULL);
        mouseinterval(0);
    }

    return win;
}

void reinit_renderer(char *file_path, Lines_renderer *line_ren)
{
    // TODO: Free the lines that r currently stored.
    free_lines(line_ren->origin);
    
    line_ren->origin      = Alloc_line_node(0);
    line_ren->count       = 0;
    line_ren->start       = line_ren->origin;
    line_ren->end         = line_ren->origin; 
    line_ren->current     = line_ren->origin; 
    line_ren->max_padding = 2;

    // If the path that was passed was a file, or if it does not exist. we assign.
    line_ren->file_type = get_file_type (file_path);
}

int load_file(char *file_path, Lines_renderer *line_ren)
{
    char c;
    FILE *Stream = fopen(file_path, "r");
    char line_number[LINE_NUM_MAX] = { 0 };

    if (line_ren->count > 0) reinit_renderer(file_path, line_ren);
    if (!Stream) { 
        line_ren->count++;
        goto SET_PROPS;
    }

    while ((c = fgetc(Stream)) != EOF) {
        if (c == '\n') {
            editor_new_line(line_ren, false);
            if (line_ren->current->y - line_ren->start->y == line_ren->win_h - MENU_HEIGHT_ ) {
                line_ren->end = line_ren->current->prev;
            }

            continue;
        }

        line_push_char(line_ren->current, c, true);
        if (!line_ren->count) line_ren->count++;
    }

    if (line_ren->count > 1 && (line_ren->start == line_ren->end)) {
        line_ren->end = line_ren->current;
    }

SET_PROPS:
    line_ren->max_padding = sprintf(line_number, "%u", line_ren->current->y + 1) + 1;
    // if (line_ren->max_padding < 1) line_ren->max_padding++;
    line_ren->current = line_ren->start;
    if (Stream) fclose(Stream);
    return line_ren->count;
}


int save_file(char *file_path, Line *lines, bool release)
{
    FILE *Stream = fopen(file_path, "w+");
    int  bytes_saved = 0;
    Line *next = NULL;
    Line *current = lines;

    for (; current;) {
        bytes_saved += fprintf(Stream, "%s\n", current->content);
        next = current->next;

        if (release)
        {
            free(current->content);
            free(current);
        }
        
        current = next;
    }

    fclose(Stream);
    return bytes_saved;
}

static void render_line(Line *line, int offset, int max_padding)
{
    int x = 0, n = 0;
    char line_number[LINE_NUM_MAX] = {0};
    line->padding = sprintf(line_number, "%u", line->y + 1);

    for (n = 0; n < line->padding; n++) {
        mvaddch(line->y - offset, n, line_number[n]);
    }
    // TODO: COLORIZE THE LINE NUMBER WITH A BETTER COLOR. (line->y - offset, 0, max_padding, PAIR, NULL);
    for (; n < max_padding; n++) {
        mvaddch(line->y - offset, n, ' ') ;
    }
    
    for (x = 0; x < line->size; ++x) {
         mvaddch(line->y - offset, x + n, line->content[x]);
    }
}

void editor_dl(Line *line) 
{
    // NORMAL del. 
    if (line->x > 0) {
        memmove(
            line->content + line->x,
            line->content + line->x + 1,
            line->size - line->x
        );
   
        line->x--;
        line->size--;
        line->content[line->size] = 0;
        return;
    }
}

void editor_backspace(Lines_renderer *line_ren)
{
    // NORMAL backspacing. 
    if (line_ren->current->x > 0) {
        memmove(
            line_ren->current->content + line_ren->current->x - 1,
            line_ren->current->content + line_ren->current->x,
            line_ren->current->size - line_ren->current->x
        );
   
        line_ren->current->content[line_ren->current->size] = 0;
        line_ren->current->x--;
        line_ren->current->size--;

        return;
    }
    
    // if the cursor is on the first col of a line and we need to shift lines and data  backward
    if (line_ren->current->x == 0 && line_ren->current->prev) {
        
        if (line_ren->current->size) {
            // move all the data locateed in the curr line to the end of the prev line!
            memmove(
                line_ren->current->prev->content + line_ren->current->prev->size,
                line_ren->current->content,
                line_ren->current->size
            );

            line_ren->current->prev->x     = line_ren->current->prev->size;
            line_ren->current->prev->size += line_ren->current->size;
        }

        line_disconnect_from_ren(line_ren);
    }

    // Don't uncomment this line, it is a fucking bug..
    // line_ren->current->content[line_ren->current->x] = 0;
}

void line_disconnect_from_ren(Lines_renderer *line_ren)
{
    line_ren->current = disconnect_line(line_ren->current); // go back one line.
    line_ren->count--;
    if (line_ren->end->next != NULL) {
        line_ren->end = (line_ren->end->next);
    }
}

static void token_highlight(MIToken *token, int y, int y_offset, int x_offset) {
    switch (token->kind) {
        case KEYWORD: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                KEYWORD_SYNTAX_PAIR);
        } break; 
        case C_INCLUDE_FILE: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                BRIGHT_YELLOW_PAIR);
        } break;
        case C_INCLUDE: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                BRIGHT_GREEN_PAIR);
        } break;
        case C_TAG: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                TAG_PAIR);
        } break;
        case STR_LIT: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                STRING_LIT_PAIR);
        } break;
        case  CALL: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                CALL_SYNTAX_PAIR);

        } break;
        case COMMENT: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                COMENT_PAIR);
        } break;
        case _GENERIC_NULL: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                NUM_PAIR);
        } break;  
        case NUMBER_LIT: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                NUM_PAIR);
        } break;
        default: {
            colorize(y - y_offset, 
                token->xstart + x_offset, 
                token->xend + x_offset, 
                MAIN_THEME_PAIR
            );
        } break;
    }
}

static void add_syntax_(Line *current, Lines_renderer *line_ren)
{
    // retokenize the line..    
    FileType file_type = line_ren->file_type;
    if (file_type == UNSUP) return; // Make sure that the script is supported..
    retokenize_line(current, file_type);
    for (int it = 0; it < (current->token_list).size; ++it) {
        token_highlight(&(current->token_list._list[it]), 
            current->y, 
            line_ren->start->y, 
            line_ren->max_padding);
    }
}

void render_lines(Lines_renderer *line_ren)
{
    Line *current = line_ren->start;
    // Update end.
    while (line_ren->end->y - line_ren->start->y < line_ren->win_h - MENU_HEIGHT_) {
        if (line_ren->end->next) {
            line_ren->end = line_ren->end->next;
        }
        break;
    }

    while (current) {
        render_line(current, line_ren->start->y, line_ren->max_padding);
        add_syntax_(current, line_ren);
        if (current == line_ren->end) break;
        current = current->next;
    }
}

void editor_details(Lines_renderer *line_ren, char *_path, editorMode mode_, char *notification)
{
    char details_buffer[LINE_SZ] = {0};
    memset(details_buffer, 0, LINE_SZ);
    char *mode = modes[mode_];

    switch (mode_)
    {
        case FILEBROWSER: {
            char *User = getenv("USER");
            sprintf(details_buffer, "#%s %s", User, _path);
            // Display the mode.
            mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
            mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLUE_PAIR, NULL);
    
            // Display notification.
            mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s", notification);
            
            // Display details.
            mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1, details_buffer);
            mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL, SECONDARY_THEME_PAIR, NULL);
        } break;
        default: {
            sprintf(details_buffer, "(%d, %d)[%d]", line_ren->current->y + 1, line_ren->current->x + 1, line_ren->count);
            mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
            mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLUE_PAIR, NULL);
            mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s %s", _path, notification);
            mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1, details_buffer);
            mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL, SECONDARY_THEME_PAIR, NULL);
            move(line_ren->current->y, line_ren->current->x);
        };
    }
}

 // isalnum,  isalpha, isascii, isblank, iscntrl, isdigit, isgraph, islower, isprint, ispunct, isspace, isupper, isxdigit, isalnum_l, isalpha_l, 
// isas‐cii_l, isblank_l, iscntrl_l, isdigit_l, isgraph_l, islower_l, isprint_l, ispunct_l, isspace_l, isupper_l, isxdigit_l  -  character  classification functions.
void char_inject(Line *line, char c)
{
    if (isalnum(c) || ispunct(c) || isspace(c)) {
        
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

void editor_new_line(Lines_renderer *line_ren, bool reset_borders)
{
    Line *new, *next;
    char line_number[LINE_NUM_MAX] = { 0 };
    int  new_max;
    new = Alloc_line_node(line_ren->current->y + 1);
    new_max = sprintf(line_number, "%u", line_ren->current->y + 1) + 1;

    if (new_max > line_ren->max_padding)
        line_ren->max_padding = new_max;

    if (!(line_ren->current->next) && line_ren->current->x == line_ren->current->size) 
    {
        line_ren->current->next = new;
        new->prev = line_ren->current;
        line_ren->current = new;
        if (reset_borders) line_ren->end = new;
        if (new->y - line_ren->start->y > line_ren->win_h - MENU_HEIGHT_ - 1 && reset_borders) {
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
        lines_shift(new->next, 1);
        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - MENU_HEIGHT_ - 1 && reset_borders) 
            line_ren->end = line_ren->end->prev;

        line_ren->current = line_ren->current->next;
        line_ren->count++;
        return;
    }
    
    // Allocate new line
    if (line_ren->current->next) {
        // prev = line_ren->current->prev;
        next = line_ren->current->next;
        new->next = next;
        next->prev = new;
        new->prev = line_ren->current;
        line_ren->current->next = new;
        new->size = line_ren->current->size - line_ren->current->x;
        line_ren->current->size -= new->size;
        
        // move memory
        memmove(new->content,
            line_ren->current->content + line_ren->current->x,
            new->size
        );

        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - MENU_HEIGHT_  - 1)
            line_ren->end = line_ren->end->prev;

        lines_shift(new->next, 1);
    } else {
        line_ren->current->next = new;
        new->prev = line_ren->current;
        new->size = line_ren->current->size - line_ren->current->x;
        line_ren->current->size -= new->size;
        memmove(new->content,
                line_ren->current->content + line_ren->current->x,
                new->size
        );

        line_ren->end = new;
        if (new->y - line_ren->start->y > line_ren->win_h - MENU_HEIGHT_ - 1 && reset_borders) {
            line_ren->start = line_ren->start->next;
        }
    }

    memset(line_ren->current->content + line_ren->current->x, 0, new->size);
    line_ren->current = line_ren->current->next;
    line_ren->count++;
}

static int evenize(const char *s) {
    int length = strlen(s);
    return (length % 2) ? length : length + 1;
}

static int editor_render_help(int x, int y, char *error)
{
    // render the welcome window in the middle of the screen.
    char *prompt = "Enter the path to the file that u want to edit: ";
    char *save_p = "[ F1 ] to save the edited file";
    char *quit_p = "[ F2 ] to quit without saving";
    
    attron(A_STANDOUT);	
    mvprintw(y, x - evenize(WLCM_BUFF)/2, WLCM_BUFF);
    attroff(A_STANDOUT);  
    mvprintw(y + 3, x - evenize(save_p)/2, save_p);
    mvprintw(y + 5, x - evenize(quit_p)/2, quit_p);
    
    if (error) {
        mvprintw(y + 7, x - evenize(error)/2, error);
        mvchgat(y + 7, x - evenize(error)/2, strlen(error), A_NORMAL, ERROR_PAIR, NULL);	
    }
    
    attron(A_UNDERLINE);
    mvprintw(y * 2, 0, prompt);
    attroff(A_UNDERLINE);

    return (strlen(prompt));
}

char *editor_render_startup(int x, int y)
{
    int prompt_offset = editor_render_help(x, y, NULL);
    Result *res = NULL;
    char *file_path = (calloc(1, LINE_SZ));
    
    while (true) {
        res = make_prompt_buffer(prompt_offset, y * 2);
        switch(res->type) {
            case SUCCESS: {
                strcpy(file_path, res->data);
                free(res->data);
                free(res);
                return file_path;
            } break;
            case ERROR: {
                if (res->etype == EXIT_SIG) {    
                    free(res->data);
                    free(res);
                    free(file_path);
                    return NULL;
                } else if (res->etype == EMPTY_BUFF) {
                    prompt_offset = editor_render_help(x, y, res->data);
                    free(res->data);
                    free(res);
                } else {
                    printf("Unreachable code\n");
                    exit(1);
                }
            } break;
            default: {
                printf("Unreachable code\n");
                exit(1);
            }
        }
    }
}


static void 
highlight__(int y, int x, int size) {
    mvchgat(y, // Which line
            x, // Which col to start from
            size, // where or howmuch to highlight after x.. which is position x + size
            A_NORMAL, 
            HIGHLIGHT_THEME, 
            NULL);
}

int highlight_until_current_col(Vec2 start, Lines_renderer *line_ren)
{
    int highlight_count = 0;
    int x = start.x, y = start.y;
    Line *current = line_ren->current;
    Line *line = NULL;

    if (y - line_ren->start->y == current->y - line_ren->start->y) {
        if (current->x > x) {
            highlight__(y - line_ren->start->y,
                x + line_ren->max_padding,
                current->x - x
            );

            highlight_count = current->x - x;
            return highlight_count;
        }
        highlight__(y - line_ren->start->y,
            current->x + line_ren->max_padding, 
            x - current->x
        );
        highlight_count = (x - current->x + 1);
        return highlight_count;
    }

    if (y < current->y) {
        
        // Highligh current.
        highlight__(y - line_ren->start->y,
            x + line_ren->max_padding,
            (start._line)->size
        );

        highlight_count += (start._line)->size;
        // iterate thro all the lines and highlight them until the current line!
        line = (start._line)->next;
        while (line != current) {
            highlight__(line->y - line_ren->start->y,
                line_ren->max_padding,
                (line)->size
            );
            line = line->next;
            highlight_count += (line)->size;
        }

        highlight__(line->y - line_ren->start->y, 
            0 + line_ren->max_padding, 
            line->x);
        highlight_count += (line)->x;
        return highlight_count;
    }
 
    if (y  > current->y) {
        
        // Highligh current.
        highlight__(y - line_ren->start->y,
            line_ren->max_padding,
            x
        );
        
        highlight_count += current->size;
        // iterate thro all the lines and highlight them until the current line!
        line = (start._line)->prev;
        while (line != current) {
            highlight__(line->y - line_ren->start->y,
                0 + line_ren->max_padding,
                (line)->size
            );
            line = line->prev;
            highlight_count += (line)->size;
        }

        highlight__(line->y - line_ren->start->y, 
            line->x + line_ren->max_padding, 
            line->size - line->x);
    
        highlight_count += line->size - line->x;
        return highlight_count;
    }

    return highlight_count;
}

static void get_string_chunk(Chunk *c, char *s, int start, int end, int size)
{
    int n = (size - start) - (size - end);
    if (n) {
        for (int it = start; it < end; ++it)
            chunk_append_char(c, s[it]);
    }
}

static int cut_data(char *buffer, int cstart, int cend, int size) {
    memmove(buffer + cstart, buffer + cend, size - cend);
    return (cend - cstart);
}


void clipboard_cut_chunk(Lines_renderer *line_ren, Vec2 start, Vec2 end) {
    Vec2  temp = { .x = start.x, .y = start.y, ._line = start._line };
    Line  *curr   = NULL;
    Chunk *chunk = chunk_new();
    int   ncut = 0;
    
    if (start.y == end.y) { // We need to copy one line!
        curr = start._line;
        int start_idx = (start.x > end.x) ? end.x : start.x;
        int end_idx   = (start.x > end.x) ? start.x : end.x;
        get_string_chunk(chunk, curr->content, start_idx, end_idx, curr->size);
        ncut = cut_data(curr->content, start_idx, end_idx, curr->size);
        curr->size -= ncut;
        curr->x    = start_idx;        
        
        goto SET_AND_EX;
    }

    if (start.y > end.y) {
        start = end;
        end = temp;
    }

    curr = start._line;
    get_string_chunk(chunk, 
        curr->content, 
        start.x, 
        curr->size, 
        curr->size
    );

    chunk_append_char(chunk, '\n');
    
    {
        ncut  = cut_data(curr->content, start.x, curr->size, curr->size);
        curr->size -= ncut;
        curr->x    = start.x;
        if (!curr->size) {
            curr = disconnect_line(curr);
            line_ren->current = curr;
            if (line_ren->current->next != NULL) {
                line_ren->current = line_ren->current->next;
                if (line_ren->end->next != NULL) {
                    line_ren->end = line_ren->end->next;
                }
            }
        }
    }

    curr = curr->next;
    
    while (curr != end._line) {
        
        get_string_chunk(chunk, 
            curr->content, 
            0,
            curr->size, 
            curr->size
        );
    
        ncut = cut_data(curr->content, 0, curr->size, curr->size);
        curr->size -= ncut;
        curr->x     = 0;
        if (!curr->size) {
            curr = disconnect_line(curr);
            line_ren->current = curr;
            if (line_ren->current->next != NULL) {
                line_ren->current = line_ren->current->next;
                if (line_ren->end->next != NULL) {
                    line_ren->end = line_ren->end->next;
                }
            }
        }

        curr = curr->next;
        chunk_append_char(chunk, '\n');
    }
    
    if (curr) {
        get_string_chunk(chunk, 
            curr->content, 
            0,
            end.x, 
            curr->size
        );

        ncut = cut_data(curr->content, 0, end.x, curr->size);
        curr->size -= ncut;
        curr->x     = 0;
        if (!curr->size) {
            curr = disconnect_line(curr);
            line_ren->current = curr;
            if (line_ren->current->next != NULL) {
                line_ren->current = line_ren->current->next;
                if (line_ren->end->next != NULL) {
                    line_ren->end = line_ren->end->next;
                }
            }
        
        }
        chunk_append_char(chunk, '\n');
    }
SET_AND_EX:
    CLIPBOARD_SET(chunk->data);
    free(chunk);
}

void clipboard_save_chunk(Vec2 start, Vec2 end)
{
    Vec2 temp = { .x = start.x, .y = start.y, ._line = start._line };
    Line *curr   = NULL;
    Chunk *chunk = chunk_new();
    // Use this to cut the current line. Line *disconnect_line(Line *head)

    if (start.y == end.y) { // We need to copy one line!
        curr = start._line;
        int start_idx = (start.x > end.x) ? end.x : start.x;
        int end_idx   = (start.x > end.x) ? start.x : end.x;
        get_string_chunk(chunk, curr->content, start_idx, end_idx, curr->size); 
        goto SET_AND_EX;
    }

    if (start.y > end.y) {
        start = end;
        end = temp;
    }

    curr = start._line;

    get_string_chunk(chunk, 
        curr->content, 
        start.x, 
        curr->size, 
        curr->size
    );

    chunk_append_char(chunk, '\n');
    curr = curr->next;
    
    while (curr != end._line) {
        
        get_string_chunk(chunk, 
            curr->content, 
            0,
            curr->size, 
            curr->size
        );
        chunk_append_char(chunk, '\n');
        curr = curr->next;
    }
    
    if (curr) {
        get_string_chunk(chunk, 
            curr->content, 
            0,
            end.x, 
            curr->size
        );
        chunk_append_char(chunk, '\n');
    }

SET_AND_EX:
    CLIPBOARD_SET(chunk->data);
    free(chunk);
}
