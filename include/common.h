#ifndef COMMON_H
#define COMMON_H
#include <string.h>
#define TEC_MOST_SIG_BIT 128

int max(int a, int b);
int min(int a, int b); 
bool file_exists(const char *fpath);
bool dir_exists(const char *folder);
char *string_dup(char *str);
char *resolve_path(char *src, char *dest);
int  decode_utf8(uint32_t utf8_bytes, char *str);
bool isprintable(char c);
void string_clean(char *s);
int make_dir(char *path);

#endif // COMMON_H
