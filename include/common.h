#ifndef COMMON_H
#define COMMON_H
#include <string.h>
#define TEC_MOST_SIG_BIT 128
#define FOLDER_UNICODE (uint32_t)(L'')

int max(int a, int b);
int min(int a, int b); 
char *string_dup(char *str);
char *resolve_path(char *src, char *dest);
int  decode_utf8(uint32_t utf8_bytes, char *str);
bool isprintable(char c);

#endif // COMMON_H
