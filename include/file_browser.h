#ifndef FILE_B_H
#define FILE_B_H

#include <dirent.h>
#include <errno.h>
#define FB_MAX_ENT 50

typedef enum BrowseEntryT {  
    FILE__ = 0x0,
    DIR__,
    NOT_EXIST
} BrowseEntryT;

typedef struct BrowseEntry {
    BrowseEntryT type;
    char *value;
} BrowseEntry;

typedef struct FileBrowser {
    size_t cap, size;
    BrowseEntry *entries;
    char *pwd;
} FileBrowser;

char **read_entire_dir(const char *dir_path);
BrowseEntryT get_entry_type(char *path);
char *join_dir(char *old, char *new);
FileBrowser *new_file_browser(const char *dir_path);
char *entry_type_as_cstr(BrowseEntryT T);

#endif //FILE_B_H
