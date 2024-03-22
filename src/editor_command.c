#include <mi.h>

static void editor_command_execute_normal(MiEditor *E, char *command) 
{
    if (strlen(command) == 1) {
        switch (*command) {
            case KEY_SAVE_: {
                // TODO: Save The whole file.
                int saved_bytes = save_file(E->fb->open_entry_path, E->renderer->origin, false);
                sprintf(E->notification_buffer, "(*) %dL %d bytes were written\n", 
                    E->renderer->count, 
                    saved_bytes);
            } break;
            
            case KEY_QUIT: {
                // TODO: EXIT
                E->exit_pressed = true; 
            } break;
            
            case KEY_QUIT_SAVE: {
                // TODO: Save, Exit.
                int saved_bytes = save_file(E->fb->open_entry_path, E->renderer->origin, false);
                sprintf(E->notification_buffer, "(*) %dL %d bytes were written\n", 
                    E->renderer->count, 
                    saved_bytes);
                E->exit_pressed = true;
            } break;
            default: {};
        }
    }
}

static void editor_command_execute_fb(MiEditor *E, char *command) 
{
    if (strlen(command) == 1) {
        switch (*command) {
            case KEY_SAVE_: {
                // TODO: Save The whole file.
                sprintf(E->notification_buffer, "(*) The editor can not save in the FILE BROWSER mode.");
            } break;
            case KEY_QUIT:
            case KEY_QUIT_SAVE: {
                // TODO: Save, Exit.
                E->exit_pressed = true;
            } break;
            default: {};
        }
    }
}

void editor_command_execute(MiEditor *E, char *command, editorMode mode) {
    switch (mode) {
        case FILEBROWSER: {
            editor_command_execute_fb(E, command);
        } break;
        case NORMAL: {
            editor_command_execute_normal(E, command);
        } break;
        default: {} break;
    }
}
