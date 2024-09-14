#include <mi.h>

void editor_up(Lines_renderer *line_ren) {
  size_t col = line_ren->current->x;

  if (line_ren->current->prev) {
    if (line_ren->current->y - line_ren->start->y == 0) {
      line_ren->start = line_ren->start->prev;
      if (line_ren->end->prev && line_ren->end->y - line_ren->start->y >
                                     line_ren->win_h - MENU_HEIGHT_) {
        line_ren->end = line_ren->end->prev;
      }
    }

    line_ren->current = line_ren->current->prev;
    if (col < (size_t)line_ren->current->size) {
      line_ren->current->x = col;
    } else {
      line_ren->current->x = line_ren->current->size;
    }
  }
}

void editor_left(Lines_renderer *line_ren) {
  if (line_ren->current->x > 0) {
    line_ren->current->x--;
  }
}

void editor_down(Lines_renderer *line_ren) {

  size_t col = line_ren->current->x;
  if (line_ren->current->next) {
    if (line_ren->current->y - line_ren->start->y ==
        line_ren->win_h - MENU_HEIGHT_) {
      line_ren->start = line_ren->start->next;
      if (line_ren->end->next) {
        line_ren->end = line_ren->end->next;
      }
    }

    line_ren->current = line_ren->current->next;

    if (col < (size_t)line_ren->current->size) {
      line_ren->current->x = col;
    } else {
      line_ren->current->x = line_ren->current->size;
    }
  }
}

void editor_right(Lines_renderer *line_ren) {
  if (line_ren->current->x < line_ren->current->size)
    line_ren->current->x++;
}

void editor_apply_move(Lines_renderer *line_ren) {
  move(line_ren->current->y - line_ren->start->y,
       line_ren->current->x + line_ren->max_padding);
}

bool is_move(int key) {
  return ((key == KEY_UP) || (key == KEY_DOWN) || (key == KEY_LEFT) ||
          (key == KEY_RIGHT)) ||
         (key == KEY_END) || (key == KEY_HOME) || (key == KEY_MOUSE) ||
         (key == L_SHIFT) || (key == R_SHIFT);
}

static void fb_handle_mv(int c, FileBrowser *fb) {
  switch (c) {
  case KEY_UP: {
    if (fb->cur_row) {
      if (fb->cur_row - fb->start == 0) {
        // TODO: ADJUST THE START AND END TO POINT TO VALID ENTRIES.
        if (fb->cur_row) {
          fb->start--;
          if (fb->end > fb->start)
            fb->end--;
        }
      }

      fb->cur_row--;
    }
  } break;
  case KEY_DOWN: {
    if (fb->cur_row < fb->size - 1)
      fb->cur_row++;

    if (fb->cur_row == fb->end + 1) {
      if (fb->end < fb->size - 1) {
        fb->start++;
        fb->end++;
      }
    }
  } break;
  }
}

static void handle_mouse(Lines_renderer *line_ren) {
  MEVENT event;

  if (getmouse(&event) == OK) {
    // Mouse was pressed in a specific pos.
    if (event.bstate & BUTTON1_PRESSED) {
      // Goto y.
      if (line_ren->current->y < event.y + line_ren->start->y) {
        while (line_ren->current->next) {
          if (line_ren->current->y == event.y + line_ren->start->y) {
            break;
          }
          line_ren->current = line_ren->current->next;
        }
      } else if (line_ren->current->y > event.y + line_ren->start->y) {
        while (line_ren->current->prev) {
          if (line_ren->current->y == event.y + line_ren->start->y) {
            break;
          }
          line_ren->current = line_ren->current->prev;
        }
      }
      // Goto x.
      line_ren->current->x =
          (event.x - line_ren->max_padding < line_ren->current->size)
              ? event.x - line_ren->max_padding
              : line_ren->current->size;
    }
  }
}

static void renderer_handle_mv(int c, Lines_renderer *line_ren) {

  char *current_char = NULL;
  charType _type;

  switch (c) {
  case KEY_UP: {
    editor_up(line_ren);
  } break;
  case KEY_DOWN: {
    editor_down(line_ren);
  } break;
  case KEY_LEFT: {
    editor_left(line_ren);
  } break;
  case KEY_RIGHT: {
    editor_right(line_ren);
  } break;
  case KEY_HOME: {
    line_ren->current->x = 0;
  } break;
  case KEY_END: {
    if (line_ren->current->size > 0)
      line_ren->current->x = line_ren->current->size;
  } break;
  case KEY_MOUSE: {
    handle_mouse(line_ren);
  } break;
  // Speedy keys..
  case L_SHIFT: {
    // TODO: Implement this
    editor_left(line_ren);
    {
      current_char = (line_ren->current->content + line_ren->current->x);

      _type = get_class(*current_char);
      current_char--;
      while (_type == get_class(*(current_char)) ||
             get_class(*(current_char)) != SPACE) {
        editor_left(line_ren);

        if (line_ren->current->x - 1 < 0)
          break;

        if (get_class(*(current_char)) == SPACE) {
          editor_right(line_ren);
          break;
        }

        current_char--;
      }
    }
  } break;
  case R_SHIFT: {
    // TODO: Implement this
    // TODO: This Solution is kinda bad, but I am leaving it this way until some
    // time when I finishe important refactoring.

    editor_right(line_ren);
    {
      current_char = (line_ren->current->content + line_ren->current->x);
      _type = get_class(*current_char);
      current_char++;
      while (_type == get_class(*(current_char)) ||
             get_class(*(current_char)) == SPACE) {
        editor_right(line_ren);

        if (line_ren->current->x + 1 >= line_ren->current->size)
          break;

        if (get_class(*(current_char)) == SPACE) {
          editor_right(line_ren);
          break;
        }

        current_char++;
      }
    }
  } break;
  default: {
  } break;
  }
}

void editor_handle_move(int c, MiEditor *E) {
  switch (E->fb->type) {
  case FILE__:
  case NOT_EXIST: {
    renderer_handle_mv(c, E->renderer);
  } break;
  case DIR__: {
    fb_handle_mv(c, E->fb);
  } break;
  default: {
  } break;
  }
}
