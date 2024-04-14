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

#define INIT_COMMAND_CAP 25
typedef struct ecommand_t {
    char *name;
    char **argv;
    size_t cap;
    size_t size;
} ecommand_t;

ecommand_t *command_alloc(size_t cap) {
    ecommand_t *c = malloc(sizeof(*c));
    memset(c, 0, sizeof(*c));
    c->argv = malloc(sizeof(char *) * cap);
    c->cap  = cap;
    c->size = 0;
    return (c);
}

void command_distroy(ecommand_t *c) {
    
    if (c == NULL) return;
    
    for (size_t i = 0; i < c->size; ++i) {
        free(c->argv + i);
    }
     
    free(c->argv);
    free(c->name);
    free(c);
}


ecommand_t *command_parse(const char *command) {
    assert(command != NULL);
    ecommand_t *c = command_alloc(INIT_COMMAND_CAP);
    // TODO: Implement this one! to parse commands, take a buff = "name argv1 argv2 argv3"
    // then return this struct { name -> "name", argv -> {"argv1", "argv2", "argv3"}, .size -> 3, .cap -> 25 }
    return c;
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
    
    // parse_cmd() 
    // if (strcmp(command) {
    //
    // }
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
