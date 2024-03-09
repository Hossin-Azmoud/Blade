#ifndef FILE_B_H
#define FILE_B_H

#include <dirent.h>
#include <errno.h>

typedef enum BrowseEntryT {  
    FILE__ = 0x0,
    DIR__
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
#endif //FILE_B_H
