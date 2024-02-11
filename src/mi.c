#include <mi.h>
// int fgetc(FILE *stream);
WINDOW *init_editor(char *file_name)
{
    int h, w;

    WINDOW *win = initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
    getmaxyx(win, h, w);

    for (int i = 0; i < w; ++i) {
        mvprintw(h - 3, i, "_");
    }

    mvprintw(h - 2, 0, "Editing %s", file_name);
    move(0, 0);
    return win;
}

size_t load_file(char *file_path, Line *lines)
{
    size_t line_count = 0;
    Line *prev = NULL;
    Line *current = lines;
    char c;
    FILE *Stream = fopen(file_path, "r");

    if (!Stream) {
        return line_count;
    }

    while((c = fgetc(Stream)) != EOF) {
        if (c == '\n') {
            prev = current;
            current = (prev + 1);
            current->y = prev->y + 1;
            line_count++;
        } else {
            current->content[current->x++] = c;
            current->size++;
        }

        if (!line_count) line_count++;
    }

    fclose(Stream);
    return line_count;
}

void save_file(char *file_path, Line *lines, size_t save_count) {
    FILE *Stream = fopen(file_path, "w+");
    
    for (size_t i = 0; i < save_count; ++i) {
        fprintf(Stream, "%s\n", lines[i].content);
    }

    fclose(Stream);
}

void render_lines(Line *lines, size_t count)
{
    Line *prev = NULL;
    size_t x = 0, y = 0;

    for (; y < count; ++y) {
        prev = (lines + y);
        for (x = 0; x < prev->size && prev->content[x]; ++x) {
            mvaddch(y, x, prev->content[x]);
        }
    }
}

void debugger(WINDOW *win, size_t x, size_t y, size_t size)
{
    int h, w;
    getmaxyx(win, h, w);
    mvprintw(h - 2, w - (12 + 20), "x -> %zu y -> %zu s -> %zu", x, y, size);
    move(y, x);
}
