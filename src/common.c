#include <mi.h>

char *string_dup(char *str) {
    char *s = calloc(1, strlen(str) + 1); 
    size_t i = 0;

    while (str[i]) {
        s[i] = str[i];
        i++;
    }

    s[i] = 0;
    return s; 
}

char *resolve_path(char *src, char *dest)
{
   char *delim = "/";
    if (!src || !dest) {
        return NULL;
    }
    char *buffer = calloc(1, strlen(src) + strlen(dest) + 2);
    buffer = strcpy(buffer, src);
    buffer = strcat(buffer, delim);
    buffer = strcat(buffer, dest);
    return (buffer);
}
