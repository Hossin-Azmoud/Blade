#include <mi.h>

bool file_exists(const char *fpath)
{
    struct stat sb;
    return (stat(fpath, &sb) == 0) && (S_ISREG(sb.st_mode));
}

bool dir_exists(const char *folder)
{
	struct stat sb;
	return (stat(folder, &sb) == 0) && (S_ISDIR(sb.st_mode));
}

BrowseEntryT get_entry_type(char *path)
{
    if (file_exists(path)) {
        return FILE__;
    }

    if (dir_exists(path)) {
        return DIR__;
    }

    return NOT_EXIST;
}

void get_entry_info(BrowseEntry *e)
{
    struct stat info;

    if (stat(e->full_path, &info)) {
        e->etype = NOT_EXIST;
        return;
    }

    if (S_ISDIR(info.st_mode)) {
        e->etype = DIR__;
    }
    if (S_ISREG(info.st_mode)) {
        e->etype = FILE__; 
    }

    e->size = info.st_size;
}

static char *All[] = {
    [FILE__] = "FILE__",
    [DIR__]  = "DIR__",
    [NOT_EXIST] = "NOT_EXIST"
};

char *entry_type_as_cstr(BrowseEntryT T)
{
    if (T <= NOT_EXIST) {
        return All[T];
    }
    return All[NOT_EXIST];
}

static FileBrowser *new_fb(const char *path) {

    FileBrowser *fb = malloc(sizeof (FileBrowser));

    fb->cur_row = 0;
    fb->cap     = FB_MAX_ENT;
    fb->entries = malloc(sizeof(BrowseEntry) * fb->cap);
    memset(fb->entries, 0, sizeof(BrowseEntry) * fb->cap);
    fb->size    = 0;
    fb->open_entry_path = malloc(sizeof(char) * PATH_MAX);
    
    {
        realpath(path, fb->open_entry_path);
        // fprintf(get_logger_file_ptr(), "p: %s %p\n", p, p);
    }
    
    fb->type    = get_entry_type(fb->open_entry_path);
    return fb;
}


void fb_append(FileBrowser *self, char *name)
{
    if (self->size + 1 >= self->cap) {
        self->cap  += 10;
        self->entries = realloc(self->entries, sizeof(sizeof(BrowseEntry) * self->cap));
    } 

    self->entries[self->size].value = string_dup(name);
    self->entries[self->size].ftype = get_file_type(name);

    {
        char *p = resolve_path(self->open_entry_path, name);
        self->entries[self->size].full_path = p;
    }

    get_entry_info(self->entries + self->size);
    self->size++;
}

void release_fb(FileBrowser *fb)
{
    // Relaase Entries.
    for (size_t x = 0; x < fb->size; x++) {
        free(fb->entries[x].value);
        free(fb->entries[x].full_path);
    }

    free(fb->open_entry_path);
    free(fb->entries);
    free(fb);
}

void load_dir_fb(FileBrowser *fb) {
    if (fb->type == DIR__) {
        char **files = read_entire_dir(fb->open_entry_path);

        while (files[fb->size] != NULL) {
            fb_append(fb, files[fb->size]);
            free(files[fb->size - 1]);
        }

        free(files);
    }
}
FileBrowser *new_file_browser(const char *dir_path)
{
    FileBrowser *fb = new_fb(dir_path);
    load_dir_fb(fb);
    return (fb);
}

FileBrowser *update_fb(FileBrowser *fb, char *new_path) {
    fb->cur_row = 0;
    fb->entries = malloc(sizeof(BrowseEntry) * fb->cap);
    memset(fb->entries, 0, sizeof(BrowseEntry) * fb->cap);
    fb->size    = 0;

    // resolve and Copy the path.
    {
        char *p = resolve_path(fb->open_entry_path, new_path);
        free(fb->open_entry_path);
        fb->open_entry_path = p;
        fprintf(get_logger_file_ptr(), "resolve: %s %p\n", fb->open_entry_path, fb->open_entry_path);
    }
     
    fb->type    = get_entry_type(fb->open_entry_path);
    load_dir_fb(fb);
    return fb;
}


FileBrowser *realloc_fb(FileBrowser *fb, char *next)
{
    update_fb(fb, next);
    return fb;
}

void fb_update(int c, MiEditor *E)
{
    switch (c) {
        case NL: {
            BrowseEntry entry = E->fb->entries[E->fb->cur_row];            
            E->fb   = realloc_fb(E->fb, entry.value);
            if (E->fb->type != DIR__) {
                E->renderer->file_type = get_file_type (E->fb->open_entry_path);
                load_file(E->fb->open_entry_path, E->renderer);
                E->mode = NORMAL;
                return;
            }
            
            E->mode = FILEBROWSER;
        } break;
        case 'd': {} break;
        case 'a': {

            // TODO: add a new file in the current dir tree.
            // Make nameBuff and pass it to fb_append.  
            curs_set(1);
            char *label = "> Create file ";
            int y = E->renderer->win_h - 2;
            mvprintw(y, 0, label);
            Result *res = make_prompt_buffer(strlen(label), y, E->renderer->win_w);

            switch(res->type) {
                case SUCCESS: {
                    fb_append(E->fb, res->data);
                    reinit_renderer(res->data, E->renderer);
                    save_file(res->data, E->renderer->origin, false);                    
                    free(res->data);
                    free(res);
                } break;
                case ERROR: {
                    if (res->etype == EXIT_SIG) {    
                        free(res->data);
                        free(res);
                    } else if (res->etype == EMPTY_BUFF) {
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
        } break;
    }
}

// void render_entry(BrowseEntry entry, int y, int x, bool colorize_) {
//     size_t padding = 5
//      
// }



void render_file_browser(MiEditor *E)
{    
    erase();
    curs_set(0);

    size_t xpadding = 5;
    size_t ypadding = 5;
    size_t row  = E->fb->cur_row;
    Emoji  *emoji = NULL; 
    // char   file_size[LINE_SZ] = {0};
    // char  *size_unit = NULL;

    if (E->fb->type == DIR__) {
        for (size_t y = 0; y < E->fb->size; y++) {
            xpadding = 5;
            BrowseEntry entry = E->fb->entries[y];  

            // TODO: render a file emoji if it is a file, otherwise render a folder emoji.
            switch (entry.etype) {
                case FILE__: {
                } break;
                case DIR__: {
                    emoji = emoji_get(E_FOLDER);
                } break;
                default: {
                    if (entry.ftype == C) {
                        emoji = emoji_get(E_C_FILE);  
                    } else {
                        emoji = emoji_get(E_FILE);  
                    }
                };
            }
    
            // Render the size of each entry.            
            mvprintw(y + ypadding, xpadding, "%s ", emoji->decoded);
            colorize(y + ypadding, xpadding, emoji->size, KEYWORD_SYNTAX_PAIR);
            xpadding += emoji->size + 1;
            

            mvprintw(y + ypadding, xpadding, entry.value);
            
            if (row == y) {
                colorize(row + ypadding, xpadding, 
                    strlen(entry.value), 
                    HIGHLIGHT_WHITE);
            }
            
            // {
            //     // Render the size of the thing.
            //     rsize = entry.size;
            //
            //     if (rsize >= 1024 && rsize < (1024 * 1024)) {
            //         size_unit = "kb";
            //         rsize /= 1024; // To get how much kb is contains.
            //     } else if (rsize >= (1024 * 1024) && rsize < (1024 * 1024) * 1024) {
            //         size_unit = "mb";
            //         rsize /= (1024 * 1024); // To get how much kb is contains.
            //     } else {
            //         size_unit = "byte";
            //     }
            //
            //     len = sprintf(file_size, "%zu %s ", rsize, size_unit);
            //     mvprintw(y + ypadding, xpadding, "%s", file_size);
            //     xpadding += len;
            // }
            // memset(file_size, 0, LINE_SZ);
        }
    }
}
