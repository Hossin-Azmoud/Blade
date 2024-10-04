#ifndef XSTRING_H
#define XSTRING_H 
char *xstrdup(char *s);
int   xstr(const char *s1, const char *s2);
char *memxcpy(char *src, int n);
char *xescape(char *src);
#endif //XSTRING_H
