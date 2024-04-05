#include <mi.h>


// #define EXP
// TODO: Implement -v/--version flags and --help
void editor_help(char *program) {
    printf("\nusage => %s <file/directory>\n", program);
    printf("flags:\n");
    printf("-h, --help    => Prints the help message into stdout.\n");
    printf("-v, --version => Prints the version of the editor.\n\n");
}

bool check_args(int argc, char **argv) 
{
    char *program = *argv;
    if (argc >= 2) {
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
            printf(MI_V);
            return true;
        }
        
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            editor_help(program); 
            return true;
        }
    }

    return false;
}

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
    if (!check_args(argc, argv)) {
        ret = editor(argv);
        CLIPBOARD_FREE();
    }
#endif /* ifdef EXP */
    return ret;
}

