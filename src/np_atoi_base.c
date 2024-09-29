#include <nparsing.h>

long np_atoi_base(char *a, char *base)
{
  long num;
  int blen;
  int sign;

  if (a == NULL)
    return (0);
  sign = 1;
  while (*a == '-' || *a == '+' || isspace(*a)) {
    if (*a == '-')
      sign *= -1;
    a++;
  }
  // NOTE: What if the base == NULL.
  if (base == NULL) {
    // NOTE: Dont forget to jump two letters with a += 2; or the lenth of the base sequence.
    base = BASE_10_ASCII;  
    if (strncmp(BASE_16_PREFIX, (const char *)a, 2) == 0) {
      base = BASE_16_ASCII;
      a += 2;
    } else if (strncmp(BASE_16_PREFIX_UP, (const char *)a, 2) == 0) {
      base = BASE_16_ASCII_UP;
      a += 2;
    } else if (strncmp(BASE_BIN_PREFIX, (const char *)a, 2) == 0) {

      base = BASE_BIN_ASCII;
      a += 2;
    }
  }

  blen = is_base_valid(base);
  if (blen < 2)
    return (0);
  num = 0;
  int sym_idx;
  while (*a) {
    sym_idx = find_sym(*a++, base);
    if (sym_idx < 0)
      break;
    num = (num * blen) + sym_idx;
  }
  return (num * sign);
}
