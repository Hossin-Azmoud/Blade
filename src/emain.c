#include <mi.h>

int editor(char **argv)
{
    setlocale(LC_CTYPE,""); 
    open_logger();
    emoji_init(); // Init the emojis cache.
    MiEditor *E = init_editor(argv[1]);
    int c = 0;
    if (E == NULL) {
        goto EXIT_AND_RELEASE_RESOURCES;
    }
    
    while ((c = getch()) != EOF && !E->exit_pressed) {
        editor_load_layout(E);

        if (is_move(c)) {
            editor_handle_move(c, E);
            goto UPDATE_EDITOR;
        }

        if (c == KEY_COMMAND_ || c == KEY_COMMAND_O) {
            if (E->mode == FILEBROWSER || E->mode == NORMAL) {
                // TODO: COMMAD MODE > <
                editor_command_(E);
                goto UPDATE_EDITOR;
            }
        }

        switch (E->fb->type) 
        {
            case DIR__: { 
                editor_file_browser(c, E); 
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

EXIT_AND_RELEASE_RESOURCES:
    endwin();
    release_editor(E);
    close_logger();
    return 0;
}
