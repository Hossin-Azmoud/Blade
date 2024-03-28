#include <mi.h>


// #define EXP

int main(int argc, char **argv) {
    (void) argc; 
    int ret = 0;

#ifdef EXP
    (void) argv;

    char *src = "for";
    Line *line    = Alloc_line_node(0);
    line->content = string_dup(src); 
    line->size    = strlen(src);

    retokenize_line(line, C);
    
    for (int i = 0; i < line->token_list.size; i++) {
        printf("(%i, %i) %s\n", 
            line->token_list._list[i].xstart,
            line->token_list._list[i].xend,
            get_token_kind_s(line->token_list._list[i].kind)
        );
    }    
#else
    ret = editor(argv);
    CLIPBOARD_FREE();
#endif /* ifdef EXP */
    return ret;
}
