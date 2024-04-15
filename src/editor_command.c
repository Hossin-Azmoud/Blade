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
    if (!*command || !command) return; // This might be dead code that never gets executed in the editor but for some security we better check.
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
    
    eCommand *cmd = command_parse(command);
    // Now we parse and look if the command is recognized.
    if (strcmp(cmd->name, "cd") == 0) {
        // TODO: make an prexesting command to execute inside the editor.
        // NOTE: since we have the editor, maybe I should call the editor_changedir(E, dir)
        // NOTE: Before doing anything I need to check if the operand is a directory.
        if (cmd->size) {
            BrowseEntryT type = get_entry_type(cmd->argv[0]);
            switch (type) {
            case DIR__: {
                // Here we change the directory.
                E->fb   = realloc_fb(E->fb, cmd->argv[0], E->renderer->win_h);
            } break;
            case NOT_EXIST: {
                // We say that it does not exist.
                sprintf(E->notification_buffer, "the arg distenation does not exist.");
            } break;
            case FILE__: {
                // We just say that it is an illegal dir type.
                sprintf(E->notification_buffer, "Expected directory not a file as arg.");
            } break;
            default: {
                sprintf(E->notification_buffer, "Illegal case at (editor_command_execute_fb)");
            } break;
            }
        }
    }

    command_distroy(cmd);
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
