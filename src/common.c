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


