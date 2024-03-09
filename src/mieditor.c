#include <mi.h>
static Vec2 vec2() {
    return (Vec2) {.x = 0, .y=0, ._line=NULL};
}

MiEditor *init_editor(char *file_path)
{
    MiEditor *E = malloc(sizeof(MiEditor));

    E->ewindow = init_ncurses_window();
    E->renderer= malloc(sizeof(Lines_renderer));
    editor_load_layout(E);

    // If the caller did not supply a file then we ask him in a seperate screen.
    if (file_path == NULL) {
        E->file = editor_render_startup(E->renderer->win_w / 2, E->renderer->win_h / 2);
    } else {
        int sz = strlen(file_path);
        E->file = malloc(sz + 1);
        E->file = memcpy(E->file, file_path, sz);
    }

    // Prepare for highlighting text (Copying and pasting..)
    E->highlighted_end         = vec2() ; // x=0, Y=0
    E->highlighted_start       = vec2() ; // x=0, Y=0
    E->highlighted_data_length = 0;

    // Prepare notification_buffer and other important properties..
    E->notification_buffer = malloc((1024 + 1) * sizeof(char));
    memset(E->notification_buffer, 0, 1024);
    E->exit_pressed = false;
    E->char_deleted = false;
    E->mode         = NORMAL;
    
    // Prepare the renderer.
    E->renderer->origin      = Alloc_line_node(0);
    E->renderer->start       = E->renderer->origin;
    E->renderer->end         = E->renderer->origin; 
    E->renderer->current     = E->renderer->origin; 
    E->renderer->script_type = get_script_type (E->file);
    E->renderer->max_padding = 2;
    
    // Load the file that was given to the editor from argv.
    load_file(E->file, E->renderer);

    // Init binding queue.    
    E->binding_queue = (vKeyBindingQueue){0x0};
    editor_refresh(E);
    // Return The editor 
    return (E);
}



void editor_load_layout(MiEditor *E)
{
    // Set the dimentions of the edittor..
    getmaxyx(E->ewindow, 
         E->renderer->win_h, 
         E->renderer->win_w
    );
}

void release_editor(MiEditor *E)
{
    if (!E) return;
    free_lines(E->renderer->origin);
    free(E->renderer);
    free(E->file);
    free(E->notification_buffer);
    free(E);
}

void editor_refresh(MiEditor *E)
{
    erase();
    render_lines(E->renderer);
    editor_details(E->renderer, E->file, E->mode, E->notification_buffer);
    editor_apply_move(E->renderer);
    refresh();
}

