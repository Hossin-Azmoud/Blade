#include <mi.h>
int editor(char **argv)
{
    setlocale(LC_CTYPE,""); 
    open_logger();
    emoji_init(); // Init the emojis cache.
    // system("reset");
    MiEditor *E = init_editor(argv[1]);
    int c = 0;
    if (E == NULL) {
        goto EXIT_AND_RELEASE_RESOURCES;
    }

	halfdelay(.5); // NOTE: wait ((1000ms/10ms) * n) => (1000ms/10ms) * 1ms => 100ms.
    while (true) {
        
        if (E->resized == 0) {
            // NOTE: Dont remove halfdelay because it is essencial for some signal interrupts, sometimes u need to wait for something to happen before
            // updating.
            c = getch();
        } else {

            endwin();
            E->ewindow = init_ncurses_window();
            // recompute the layout, width and height!
            editor_load_layout(E);
            // re-init the file browser..    
            if (E->fb->type == DIR__) 
                reinit_fb_bounds(E->fb, E->renderer->win_h);

            sprintf(E->notification_buffer, "resize detected!");
            E->resized = 0;
            goto UPDATE_EDITOR;
        }

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
    release_editor(E);
    close_logger();
    return 0;
}
