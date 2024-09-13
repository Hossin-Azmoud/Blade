#ifndef FILE_B_H
#define FILE_B_H
// TODO: Use a file system instead of a file browser.
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <filessystem.h>

#define FB_MAX_ENT 127

typedef struct BrowseEntry {
  bool selected; 
  BrowseEntryT etype;       // FILE__, DIR__, NOT_EXIST.
  FileType     ftype;       // C, PYTHON, JS...
  size_t       size;        // Size of the file or the directory...
  char         *full_path; // /usr/foo 
  char         *value;     // foo
} BrowseEntry;

// typedef struct FileBrowser FileBrowser;

typedef struct FileBrowser {
  size_t cap, size, cur_row, start, end;
  BrowseEntry *entries;
  BrowseEntryT type;
  char *open_entry_path; // current working path. it can the currrent open dir, or current open file.
  char *rel_path; // Not the Real path but the ralative one 
} FileBrowser;

BrowseEntryT get_entry_type(char *path);
char **read_entire_dir(const char *dir_path);
void get_entry_info(BrowseEntry *e);
FileBrowser *new_file_browser(const char *dir_path, size_t window_height);
char *entry_type_as_cstr(BrowseEntryT T);
void release_fb(FileBrowser *fb);
FileBrowser *realloc_fb(FileBrowser *fb, char *next, size_t window_height);
void fb_append(FileBrowser *self, char *name);
bool fb_exists(FileBrowser *self, char *item);
void reinit_fb_bounds(FileBrowser *fb, size_t window_height);
void remove_entry(FileBrowser *fb);
void remove_entry_by_index(FileBrowser *fb, size_t index);

#endif //FILE_B_H


