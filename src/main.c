#include <mi.h>


// #define EXP


int main(int argc, char **argv) {
    (void) argc; 
    int ret = 0;

#ifdef EXP
    (void) argv; 

{ // Make dir.
    Path *p = path_alloc(32);
    parse_path(p, "hello/");
    pprint(p);
    editor_make_apply_path_tree(p); // -> makes a file ./hello/x 
    release_path(p);
} 

#else
    ret = editor(argv);
    CLIPBOARD_FREE();
#endif /* ifdef EXP */
    return ret;
}
