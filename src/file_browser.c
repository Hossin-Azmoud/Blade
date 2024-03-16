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

char *join_dir(char *old, char *new)
{
    (void) old, (void) new;
    return NULL;
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


void release_fb(FileBrowser *fb)
{
    // Relaase Entries.
    for (size_t x = 0; x < fb->size; x++) {
        free(fb->entries[x].value);
    }

    // free(fb->open_entry_path);
    free(fb->entries);
    free(fb);
}

void load_dir_fb(FileBrowser *fb) {
    if (fb->type == DIR__) {
        char **files = read_entire_dir(fb->open_entry_path);

        while (files[fb->size] != NULL) {
            fb->entries[fb->size].type      = get_entry_type(files[fb->size]);
            fb->entries[fb->size].value     = string_dup(files[fb->size]);
            free(files[fb->size++]);
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
