#ifndef COMMON_H
#define COMMON_H
#include <string.h>
#include <filessystem.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#define TEC_MOST_SIG_BIT 128

int   max(int a, int b);
int   min(int a, int b); 
bool  file_exists(const char *fpath);
bool  dir_exists(const char *folder);
char  *string_dup(char *str);
char  *resolve_path(char *src, char *dest);
int    decode_utf8(uint32_t utf8_bytes, char *str);
bool   isprintable(char c);
void   string_clean(char *s);
int    make_dir(char *path);
size_t fsize(FILE *stream); 
char   *slurp_file_content(const char *path);
char *join_path(char *left, char *right);

#endif // COMMON_H

