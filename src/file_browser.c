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
 
    fb->cap     = FB_MAX_ENT;
    fb->entries = malloc(sizeof(BrowseEntry) * fb->cap);
    memset(fb->entries, 0, sizeof(BrowseEntry) * fb->cap);
    fb->size = 0;
    fb->pwd  = malloc(sizeof (path));
    fb->pwd  = strcpy(fb->pwd, path);

    return fb;
}

// void release_fb(FileBrowser *fb)
// {
//     // Relaase Entries.
//     for (size_t x = 0; x < fb->size; x++) {
//         free(fb->entries[x].value);
//         free(fb->entries[x]);
//     }
//
//     free(fb->entries);
// }

FileBrowser *new_file_browser(const char *dir_path)
{
    FileBrowser *fb = new_fb(dir_path);
    BrowseEntryT t = get_entry_type(fb->pwd);

    if (t == DIR__) {
        char **files = read_entire_dir(fb->pwd);

        for (size_t i = 0; (files[i]); ++i, fb->size++) {
            fb->entries[i].type  = get_entry_type(files[i]);
            fb->entries[i].value = malloc(sizeof(files[i]));
            fb->entries[i].value = strcpy(fb->entries[i].value, files[i]);
            free(files[i]);
        }

        free(files);
    }

    return (fb);
}
