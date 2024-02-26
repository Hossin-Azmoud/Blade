#include <mi.h>
// isalnum,  isalpha, isascii, isblank, iscntrl, isdigit, isgraph, islower, isprint, ispunct, isspace, isupper, isxdigit, isalnum_l, isalpha_l, isas‐cii_l, isblank_l, iscntrl_l, isdigit_l, isgraph_l, islower_l, isprint_l, ispunct_l, isspace_l, isupper_l, isxdigit_l  -  character  classification functions

charType get_class(int key)
{
    if (isdigit(key)) return NUMBER;
    if (isalpha(key)) return ALPHABET;
    if (isspace(key)) return SPACE;
    if (ispunct(key)) return SYMBOL;
    return UNK;
}
