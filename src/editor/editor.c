// #include "filessystem.h"
#include <mi.h>

static Vec2 vec2(void) { return (Vec2){.x = 0, .y = 0, ._line = NULL}; }

static void init_window(MiEditor *E) {
  // int it = 0;
  // it = 0;

  E->renderer->win_w = 0;
  E->renderer->win_h = 0;

  // log_into_f("[FIRST %i] w => %i h => %i\n", it, E->renderer->win_w,
  // E->renderer->win_h);

  // NOTE: This bootstrap function maybe is neccessary for some terminals.
  while (E->renderer->win_w == 0 && E->renderer->win_h == 0) {

    E->ewindow = init_ncurses_window();
    editor_load_layout(E);
    // mvprintw(0, 0, "[TERM %i] w => %i h => %i\n", it, E->renderer->win_w,
    // E->renderer->win_h); log_into_f("[ITERATION %i] w => %i h => %i\n", it,
    // E->renderer->win_w, E->renderer->win_h);
  }

  // log_into_f("[FINAL %i] w => %i h => %i\n", it, E->renderer->win_w,
  // E->renderer->win_h);
}

MiEditor *init_editor(char *path) {
  MiEditor *E = malloc(sizeof(MiEditor));
  init_signals();
  memset(E, 0, sizeof(*E));
  char *pathBuff = NULL;
  E->renderer = malloc(sizeof(*(E->renderer)));
  memset(E->renderer, 0, sizeof(*(E->renderer)));
  init_window(E);
  // If the caller did not supply a file then we ask him in a seperate screen.
  if (path == NULL) {
    pathBuff = editor_render_startup(
        E->renderer->win_w / 2, E->renderer->win_h / 2, E->renderer->win_w);
    if (pathBuff == NULL) {
      release_editor(E);
      return NULL;
    }
  } else {
    pathBuff = string_dup(path);
  }

  E->fb = new_file_browser(pathBuff, E->renderer->win_h);
  free(pathBuff);
  // Prepare for highlighting text (Copying and pasting..)
  E->highlighted_end = vec2();   // x=0, Y=0
  E->highlighted_start = vec2(); // x=0, Y=0
  E->highlighted_data_length = 0;

  // Prepare notification_buffer and other important properties..
  E->notification_buffer = malloc((1024 + 1) * sizeof(char));
  memset(E->notification_buffer, 0, 1024);
  E->exit_pressed = false;
  E->char_deleted = false;
  E->mode = NORMAL;

  // Prepare the renderer.

  E->renderer->origin = Alloc_line_node(0);
  E->renderer->count = 0;
  E->renderer->start = E->renderer->origin;
  E->renderer->end = E->renderer->origin;
  E->renderer->current = E->renderer->origin;
  E->renderer->max_padding = 2;

  // If the path that was passed was a file, or if it does not exist. we assign.
  if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
    E->renderer->file_type = get_file_type(E->fb->open_entry_path);
    if (E->renderer->file_type == MP3) {
      editor_init_player_routine(E, E->fb->open_entry_path);
      E->fb = realloc_fb(E->fb, "..", E->renderer->win_h);
      E->mode = FILEBROWSER;
    } else {
      load_file(E->fb->open_entry_path, E->renderer);
    }
  }

  // Init binding queue.
  E->binding_queue = (vKeyBindingQueue){0x0};
  if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
    editor_refresh(E);
  } else {
    E->mode = FILEBROWSER;
    render_file_browser(E);
    editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
                          E->notification_buffer);
  }

  editor_register_(E);
  return (E);
}

MiEditor *editor_register_(MiEditor *E) {
  static MiEditor *E_ = NULL;

  if (E != NULL && E_ == NULL) {
    E_ = E;
  }

  return E_;
}

MiEditor *editor_get(void) { return editor_register_(NULL); }

void editor_load_layout(MiEditor *E) {
  // Set the dimentions of the edittor..
  int h, w;
  getmaxyx(E->ewindow, h, w);
  E->renderer->win_h = h;
  E->renderer->win_w = w;
}

void release_editor(MiEditor *E) {
  if (!E)
    return;
  release_fb(E->fb);
  if (E->renderer) {
    free_lines(E->renderer->origin);
    free(E->renderer);
  }

  endwin();
  free(E->notification_buffer);
  if (E->selected)
    free(E->selected);
  free(E);
}

void editor_refresh(MiEditor *E) {
  erase();
  render_lines(E->renderer);
  editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
                        E->notification_buffer);
  editor_apply_move(E->renderer);
  refresh();
}

void editor_update(int c, MiEditor *E) {
  // Globals.
  switch (c) {
  case ESC: {
    E->mode = NORMAL;
    return;
  } break;
  }
  // Actions that depend on the mode.
  switch (E->mode) {
  case VISUAL: {
    editor_visual(c, E);
  } break;
  case NORMAL: {
    editor_normal(c, E);
  } break;
  case INSERT: {
    editor_insert(c, E);
  } break;
  default: {
  } break;
  }
}
