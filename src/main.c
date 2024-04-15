#include <mi.h>


// #define EXP
#define INIT_SCRIPT_PATH "init.mi"

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
    eCommand *command = command_parse("cd dir");
    if (command) {
        printf("NAME: %s\n", command->name);
        for (size_t j = 0; j < command->size; j++) {
            printf("v%zu: %s\n", j, command->argv[j]);
        }
        printf("\n");
    }
    command_distroy(command);
#else
    if (!check_args(argc, argv)) {
        ret = editor(argv);
        CLIPBOARD_FREE();
    }
#endif /* ifdef EXP */
    return ret;
}
