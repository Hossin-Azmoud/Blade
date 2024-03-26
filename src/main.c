#include <mi.h>


// #define EXP


int main(int argc, char **argv) {
    (void) argc; 
    int ret = 0;
#ifdef EXP
    (void) argv; 
#else
    ret = editor(argv);
    CLIPBOARD_FREE();
#endif /* ifdef EXP */
    return ret;
}
