#include <mi.h>

char *xstrdup(char *s) {
  size_t size = strlen(s);
  char *dup = malloc(size + 1);
  size_t idx = 0;
  for (; idx < size; idx++) {
    dup[idx] = s[idx];
  }
  dup[idx] = 0;
  return (dup);
}

int xstrcmp(const char *s1, const char *s2) {
  char c1 = 0;
  char c2 = 0;
  int ln  = 0;
  
  for (;*s2; s2++) {
    c1 = toupper(*s1);
    c2 = toupper(*s2);
    if (c1 == c2)
      break;
  }
  if (!c1 && !c2)
    return (1);
  for (;*s1 && *s2; s1++, s2++) {
    c1 = toupper(*s1);
    c2 = toupper(*s2);
    if (c1 != c2)
      break;
    ln++;
  }
  if (!*s1 && ln) {
    return (0);
  }
  return (c1 - c2);
}
