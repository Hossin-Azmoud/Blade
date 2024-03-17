#include <mi.h>

#define T 0

// #define DEBUG
int editor(char **argv);
int test(char **argv);

int main(int argc, char **argv) {
    (void) argc; 
    if (T) return test(argv);
    int ret = editor(argv);
    CLIPBOARD_FREE();
    return ret;
}

void print_token_list(TokenList *list)
{
    
    for (int i = 0; i < list->size; ++i) {
        printf("(s: %d e: %d) - %s\n", 
           list->_list[i].xstart, 
           list->_list[i].xend,
           get_token_kind_s(list->_list[i].kind)
        );
    }
}


int test(char **argv) {
    (void) argv;
    
    // DO NOTHING.
    char *p = resolve_path("/src/x/y", ".");
    
    if (p) {
        printf("%s\n", p);
        free(p);
    }
    // STRIP ONE DIR BACK
    p = resolve_path("/src/x/y", "..");
    
    if (p) {
        printf("%s\n", p);
        free(p);
    }

    // JOIN DIR 1, with 2
    // resolve_path(1, 2);
    p = resolve_path("/src/x/y", "z");
    
    if (p) {
        printf("%s\n", p);
        free(p);
    }
    
    return 0;
}

int editor(char **argv)
{
    
    open_logger();
    MiEditor *E = init_editor(argv[1]);
    int c = 0;
    if (E == NULL) {
        goto EXIT_AND_RELEASE_RESOURCES;
    }
    
    while ((c = getch()) != KEY_F(1) && E->exit_pressed != true) {
        editor_load_layout(E);

        if (is_move(c)) {
            handle_move(c, E);
            goto UPDATE_EDITOR;
        }
        
        switch (E->fb->type) 
        {
            case DIR__: {
                fb_update(c, E);    
            } break;
            case FILE__:
            case NOT_EXIST: {
                editor_update(c, E);
            } break;
            default: {} break;
        }
    UPDATE_EDITOR:
        editor_render(E);
        if (E->exit_pressed) break;
    }

    if (!E->exit_pressed) {
        if (E->fb->type != DIR__) {
            save_file(
                E->fb->open_entry_path, 
                E->renderer->origin, 
                false);
        }
    }

EXIT_AND_RELEASE_RESOURCES:
    endwin();
    release_editor(E);
    open_logger();
    return 0;
}

