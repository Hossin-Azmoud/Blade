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
    if (!strcmp(dest, "..")) {
        // TODO: remove one entry back. /include/x/y => /include/x    
        size_t last = strlen(src) - 1;

        do {
            last--;
        } while (last > 0 && src[last] != *delim);
        
        if (src[last] == *delim) last--;
        buffer = strncpy(buffer, src, last + 1);
        return buffer;
    }    

    if (!strcmp(dest, ".")) {
        // TODO: do nothing
        buffer = strcpy(buffer, src);
        return buffer;
    }

    buffer = strcpy(buffer, src);
    buffer = strcat(buffer, delim);
    buffer = strcat(buffer, dest);

    return (buffer);
}
