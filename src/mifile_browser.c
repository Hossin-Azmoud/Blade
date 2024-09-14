#include <mi.h>
#include <stdlib.h>

static char *All[] = {
    [FILE__] = "FILE__", [DIR__] = "DIR__", [NOT_EXIST] = "NOT_EXIST"};

BrowseEntryT get_entry_type(char *path) {
  if (file_exists(path)) {
    return FILE__;
  }

  if (dir_exists(path)) {
    return DIR__;
  }

  return NOT_EXIST;
}

void get_entry_info(BrowseEntry *e) {
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

char *entry_type_as_cstr(BrowseEntryT T) {
  if (T <= NOT_EXIST) {
    return All[T];
  }
  return All[NOT_EXIST];
}

static FileBrowser *new_fb(const char *path) {

  FileBrowser *fb = malloc(sizeof(FileBrowser));

  fb->cur_row = 0;
  fb->cap = FB_MAX_ENT;
  fb->entries = malloc(sizeof(BrowseEntry) * fb->cap);
  memset(fb->entries, 0, sizeof(BrowseEntry) * fb->cap);

  fb->size = 0;
  fb->open_entry_path = malloc(sizeof(char) * PATH_MAX);

  {
    realpath(path, fb->open_entry_path);
    // fprintf(get_logger_file_ptr(), "p: %s %p\n", p, p);
  }

  fb->type = get_entry_type(fb->open_entry_path);
  return fb;
}

void fb_append(FileBrowser *self, char *name) {
  if (!name)
    return;
  if (self->size + 1 >= self->cap) {
    self->cap += 10;
    self->entries = realloc(self->entries, sizeof(BrowseEntry) * self->cap);
  }

  self->entries[self->size].value = string_dup(name);
  self->entries[self->size].ftype = get_file_type(name);

  {
    char *p = resolve_path(self->open_entry_path, name);
    self->entries[self->size].full_path = string_dup(p);
    free(p);
  }

  get_entry_info(self->entries + self->size);
  self->size++;
}

void release_fb(FileBrowser *fb) {
  if (!fb)
    return;

  // Relaase Entries.
  for (size_t x = 0; x < fb->size; x++) {
    free(fb->entries[x].value);
    free(fb->entries[x].full_path);
  }

  free(fb->open_entry_path);
  free(fb->entries);
  free(fb);
}

bool fb_exists(FileBrowser *self, char *item) {
  for (size_t i = 0; i < self->size; ++i) {
    if (strcmp(self->entries[i].value, item) == 0) {
      return true;
    }
  }
  return false;
}

static int entry_cmp(const void *ap, const void *bp) {
  const char *a = (const char *)((BrowseEntry *)ap)->value;
  const char *b = (const char *)((BrowseEntry *)bp)->value;
  return strcmp(a, b);
}

void load_dir_fb(FileBrowser *fb) {
  if (fb->type == DIR__) {
    chdir(fb->open_entry_path);
    DIR *dir = NULL;
    errno = 0;
    dir = opendir(fb->open_entry_path);

    // If the dir does not exist, we make it lol.
    if (dir == NULL && errno == ENOENT) {
      make_dir(fb->open_entry_path);
      dir = opendir(fb->open_entry_path);
    }

    errno = 0;
    struct dirent *ent = readdir(dir);

    for (; ent != NULL;) {
      fb_append(fb, ent->d_name);
      ent = readdir(dir);
    }

    qsort(fb->entries, fb->size, sizeof(*fb->entries), entry_cmp);
    if (errno == 0)
      closedir(dir);
  }
}

FileBrowser *new_file_browser(const char *dir_path, size_t window_height) {
  FileBrowser *fb = new_fb(dir_path);
  load_dir_fb(fb);
  fb->start = 0;
  if (fb->size >= (window_height - MENU_HEIGHT_ - 6)) {
    fb->end = (window_height - MENU_HEIGHT_ - 6);
    return (fb);
  }
  fb->end = fb->size - 1;
  return (fb);
}

void reinit_fb_bounds(FileBrowser *fb, size_t window_height) {
  fb->start = 0;
  if (fb->size >= (window_height - MENU_HEIGHT_) - 6) {
    fb->end = (window_height - MENU_HEIGHT_ - 6);
    return;
  }
  fb->end = fb->size - 1;
}

FileBrowser *update_fb(FileBrowser *fb, char *new_path) {

  fb->cur_row = 0;
  memset(fb->entries, 0, sizeof(BrowseEntry) * fb->cap);
  fb->size = 0;

  // resolve and Copy the path.
  {
    char *p = resolve_path(fb->open_entry_path, new_path);
    free(fb->open_entry_path);
    fb->open_entry_path = p;
    // fprintf(get_logger_file_ptr(), "resolve: %s %p\n", fb->open_entry_path,
            // (void *)fb->open_entry_path);
  }

  fb->type = get_entry_type(fb->open_entry_path);
  load_dir_fb(fb);
  // reinit_fb_bounds(fb, window_height);

  return fb;
}

FileBrowser *realloc_fb(FileBrowser *fb, char *next, size_t window_height) {
  update_fb(fb, next);
  reinit_fb_bounds(fb, window_height);
  return fb;
}
void remove_entry_by_index(FileBrowser *fb, size_t index) {
  if (fb->size) {
    memmove(fb->entries + index, fb->entries + index + 1,
            sizeof(fb->entries[0]) * (fb->size - index));

    if (fb->cur_row == fb->end) {
      fb->cur_row--;
    }

    fb->size--;
    fb->end--;
  }
}
void remove_entry(FileBrowser *fb) { remove_entry_by_index(fb, fb->cur_row); }
