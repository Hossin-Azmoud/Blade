#include <ctype.h>
#include <file_browser.h>
#include <mi.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void editor_visual(int c, MiEditor *E) {
  switch (c) {
  // unCapitalize COMMAND.
  case SHIFT(KEY_CAP): {
    E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
    E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
    E->highlighted_end._line = E->renderer->current;
    // TODO: UnCapitalize region
    capitalize_region(E->highlighted_start, E->highlighted_end);
  } break;
  // Capitalize COMMAND.
  case KEY_CAP: {
    E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
    E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
    E->highlighted_end._line = E->renderer->current;
    // TODO: Capitalize region
    uncapitalize_region(E->highlighted_start, E->highlighted_end);
  } break;

  case KEY_COPY_: {
    // Store into about end of the highlighting in some struct.
    E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
    E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
    E->highlighted_end._line = E->renderer->current;
    E->mode = NORMAL;
    clipboard_save_chunk(E->highlighted_start, E->highlighted_end);
  } break;
  case 'd':
  case KEY_CUT_: {
    // Store into about end of the highlighting in some struct.
    E->highlighted_end.x = E->renderer->current->x; // x=0, Y=0
    E->highlighted_end.y = E->renderer->current->y; // x=0, Y=0
    E->highlighted_end._line = E->renderer->current;
    E->mode = NORMAL;
    // Since we are cutting lines so a region of the render_lines, or the lines
    // as referred in the struct will be kille E->renderer->current =
    // clipboard_save_chunk(E->highlighted_start, E->highlighted_end, true);
    clipboard_cut_chunk(E->renderer, E->highlighted_start, E->highlighted_end);
  } break;
  case '>': {
    Line *start = E->highlighted_start._line;
    Line *end = E->renderer->current;
    indent_lines(start, end);
  } break;
  case '<': {
    Line *start = E->highlighted_start._line;
    Line *end = E->renderer->current;
    unindent_lines(start, end);
  } break;
  default: {
  } break;
  }
}

void editor_normal(int c, MiEditor *E) {
  switch (c) {
  case SHIFT('c'): {
    E->highlighted_start = (Vec2){.x = E->renderer->current->x,
                                  .y = E->renderer->current->y,
                                  ._line = E->renderer->current};
    E->highlighted_end = (Vec2){.x = E->renderer->current->size,
                                .y = E->renderer->current->y,
                                ._line = E->renderer->current};
    clipboard_cut_chunk(E->renderer, E->highlighted_start, E->highlighted_end);
  } break;
  case SHIFT(KEY_COPY_): {
    E->highlighted_start = (Vec2){.x = E->renderer->current->x,
                                  .y = E->renderer->current->y,
                                  ._line = E->renderer->current};

    E->highlighted_end = (Vec2){.x = E->renderer->current->size,
                                .y = E->renderer->current->y,
                                ._line = E->renderer->current};
    // TODO: Copy instead of cut the lines.
    clipboard_save_chunk(E->highlighted_start, E->highlighted_end);
    sprintf(E->notification_buffer, "%i bytes coppied to clip",
            E->renderer->current->size - E->renderer->current->x);
  } break;
  case KEY_DOT: {
    save_file(E->fb->open_entry_path, E->renderer->origin, false);
    E->fb = realloc_fb(E->fb, "..", E->renderer->win_h);
    if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
      load_file(E->fb->open_entry_path, E->renderer);
      E->mode = NORMAL;
    } else {
      E->mode = FILEBROWSER;
    }
  } break;
  case KEY_PASTE_: {
    // TODO: Make clipboard be synced with the VISUAL mode clipboard_
    editor_push_data_from_clip(E->renderer);
  } break;
  case KEY_INSERT_: {
    E->mode = INSERT;
  } break;
  case KEY_VISUAL_: {
    // we mark the chords of the start position!
    E->mode = VISUAL;
    E->highlighted_start.x = E->renderer->current->x;
    E->highlighted_start.y = E->renderer->current->y;
    E->highlighted_start._line = E->renderer->current;
  } break;
  default: {
    if (E->binding_queue.size < MAX_KEY_BINDIND) {
      E->binding_queue.keys[E->binding_queue.size++] = (char)c;
      // TODO: Make binding copy whole line with `yy`, and delete whole line
      // with `dd`
    }
  } break;
  }
}

void editor_insert(int c, MiEditor *E) {
  switch (c) {
  case TAB: {
    editor_tabs(E->renderer->current);
  } break;
  case KEY_BACKSPACE: {
    editor_backspace(E->renderer);
    E->char_deleted = true;
  } break;
  case KEY_DEL: {
    // TDO: Implement this.
    editor_dl(E->renderer->current);
    E->char_deleted = true;
  } break;
  case NL: {
    editor_new_line(E->renderer, true);
    if (!E->renderer->count)
      E->renderer->count++;
    return;
  } break;
  default: {
    if (isprint(c)) {
      line_push_char(E->renderer->current, c, false);
      if (E->renderer->count)
        E->renderer->count++;
    }
  } break;
  }
}

void editor_new_entry(char *path, MiEditor *E) {
  int tree_head_idx = 0;
  Path *p = path_alloc(32);
  parse_path(p, path);
  // pprint(p);
  editor_make_apply_path_tree(p);

  if (strcmp(p->items[0], "..") == 0 ||
      strcmp(p->items[0], ".") == 0) { // Handles Only relative paths.
    tree_head_idx = 1;
  }

  // TODO: Handle abs paths.
  if (!fb_exists(E->fb, p->items[tree_head_idx])) {
    fb_append(E->fb, p->items[tree_head_idx]);
    fix_layout_file_browser(E->fb, (size_t)(E->renderer->win_h));
  }

  release_path(p);
}

void remove_entry_(MiEditor *E, size_t index, bool notified) {
  char label[4096];
  BrowseEntry entry = E->fb->entries[index];
  if (notified) {
    if (entry.etype == FILE__) {
      // TODO: Remove the current entry from the file system.
      // FUNC: remove(fullP)
      remove(entry.full_path);
      // TODO: Remove the current entry from the fb->entries
      // FUNC: void remove_entry(FileBrowser *fb)
      remove_entry_by_index(E->fb, index);
      return;
    }
    // TODO: Do the same thing for the Directories.
    sprintf(E->notification_buffer, "Can not delete Directories yet!");
    return;
  }
  int y = E->renderer->win_h - 2;
  sprintf(label,
          "> Really wanna do that (delete: %s)(y|Y|n|N): ", entry.full_path);
  mvprintw(y, 0, "%s", label);
  Result *res =
      make_prompt_buffer(strlen(label), y, E->renderer->win_w, DRACULA_PAIR);

  switch (res->type) {
  case SUCCESS: {
    if (!strcmp(res->data, "y") || !strcmp(res->data, "Y")) {
      if (entry.etype == FILE__) {
        // TODO: Remove the current entry from the file system.
        // FUNC: remove(fullP)
        remove(entry.full_path);
        // TODO: Remove the current entry from the fb->entries
        // FUNC: void remove_entry(FileBrowser *fb)
        remove_entry_by_index(E->fb, index);
        // remove_entry (E->fb);
        sprintf(E->notification_buffer, "%s was deleted successfully",
                res->data);
        return;
      }
      // TODO: Do the same thing for the Directories.
      sprintf(E->notification_buffer, "Can not delete Directories yet!");
    }
    free(res->data);
    free(res);
  } break;
  case ERROR: {
    if (res->etype == EXIT_SIG) {
      free(res->data);
      free(res);
    } else if (res->etype == EMPTY_BUFF) {
      free(res->data);
      free(res);
    }
  } break;
  default: {
    printf("Unreachable code\n");
    abort();
  }
  }
}

void editor_file_browser(int c, MiEditor *E) {
  char label[4096] = {0};
  static int findex    = -1;
  static int flist[64] = {-1};
  // NOTE: I will use this for caching selected or important entries.
  static bool marking_mode = false;
  
  switch (c) {
  case SHIFT('s'):
  case 's': {
    BrowseEntry *e = (E->fb->entries + E->fb->cur_row);
    if (strcmp(e->value, ".") == 0 || strcmp(e->value, "..") == 0)
      return;

    if (E->selected == NULL) {
      E->selected_cap = FB_MAX_ENT;
      E->selected = malloc(sizeof(*E->selected) * E->selected_cap);
    }
    if (E->selected_size >= E->selected_cap) {
      E->selected_cap += FB_MAX_ENT;
      E->selected =
        realloc(E->selected, sizeof(*E->selected) * E->selected_cap);
    }
    if (!e->selected) {
      memcpy(E->selected + E->selected_size, e, sizeof(*E->selected));
      e->index_in_selection = E->selected_size;
      E->selected_size++;
      sprintf(E->notification_buffer, "$SELECTED: %zu $SELECTED_IDX_ADD: %zu",
              E->selected_size, e->index_in_selection);
    } else {
      memmove(E->selected + e->index_in_selection - 1,
              E->selected + e->index_in_selection,
              (E->selected_size - e->index_in_selection) * sizeof(*e));
      E->selected_size--;
    }

    e->selected = !e->selected;
    marking_mode = true;
  } break;
  case 'n': {
    if (findex != -1) {
      if (flist[findex + 1] != -1) {
         findex++;
      } else {
         findex--;
      }
     
      E->fb->cur_row = flist[findex];
    }
  } break;
  case 'p': {
    if (findex != -1) {

      if (findex > 0)
        findex--;
      else
        findex++;

      E->fb->cur_row = flist[findex];
    }
  } break;
  case NL: {
    BrowseEntry entry = E->fb->entries[E->fb->cur_row];
    FileType ft = entry.ftype;
    E->fb = realloc_fb(E->fb, entry.value, E->renderer->win_h);

    if (E->fb->type != DIR__) {
      if (ft == MP3) {
        editor_init_player_routine(E, E->fb->open_entry_path);
        // after the editor finished we need to
        E->fb = realloc_fb(E->fb, "..", E->renderer->win_h);
        E->mode = FILEBROWSER;
      } else {
        E->renderer->file_type = get_file_type(E->fb->open_entry_path);
        load_file(E->fb->open_entry_path, E->renderer);
        E->mode = NORMAL;
      }
      return;
    }

    E->mode = FILEBROWSER;
  } break;
  case '/': {
    // TODO: Make a list that corresponds to the list of similar entries.
    // enable the user to cycle thro them. select one of em.
    int idx = 0;
    for (int i = 0; i < 64; i++)
      flist[i] = -1;

    curs_set(1);
    int y = E->renderer->win_h - 2;
    sprintf(label, "%s", "search > ");
    mvprintw(y, 0, "%s", label);
    Result *res =
        make_prompt_buffer(strlen(label), y, E->renderer->win_w, DRACULA_PAIR);
    switch (res->type) {
    case SUCCESS: {
      uint8_t found = 0;
      for (size_t i = 0; i < E->fb->size; i++) {
        if (xstr(res->data, E->fb->entries[i].value) == 0) {
          // TODO: For now I will just store it here.
          // I will find a way to use it.
          flist[idx++] = i;
          found = 1;
        }
      }
      if (found) {
        findex = 0;
        E->fb->cur_row = flist[findex];
        sprintf(E->notification_buffer, "<n>: Next <p>: prev");
      } else {
        findex = -1;
        sprintf(E->notification_buffer, "Entry was not found.");
      }
    } break;
    case ERROR: {
      sprintf(E->notification_buffer, "%s", res->data);
    } break;
    default: {
    };
    }

    free(res->data);
    free(res);
  } break;
  case 'd': {
    // BUG: I will fix this bug. when we remove some entry we forget
    // we need to remove it also from the selected stuff.
    curs_set(1);
    if (marking_mode) {
      int y = E->renderer->win_h - 2;
      sprintf(label, "%s",
              "> Sure wanna procced to delete the selected files: ");
      mvprintw(y, 0, "%s", label);
      Result *res = make_prompt_buffer(strlen(label), y, E->renderer->win_w,
                                       DRACULA_PAIR);
      // TODO: instead of having to check the selected field maybe it is better
      // to store them in a local static list
      switch (res->type) {
      case SUCCESS: {
        if (!strcmp(res->data, "y") || !strcmp(res->data, "Y")) {
          for (size_t i = 0; i < E->fb->size; i++) {
            if (E->fb->entries[i].selected) {
              remove_entry_(E, i, true);
              i--;
            }
          }
        }
      }
      default: {
      };
      }
      
      marking_mode = false;
      free(res->data);
      free(res);
      // E->fb = realloc_fb(E->fb, ".", E->renderer->win_h);
      return;
    }
    
    remove_entry_(E, E->fb->cur_row, false);
  } break;
  case SHIFT('m'):
  case 'm': {
    // TODO: The move function will just create a question buffer, ask where u
    // want to move the selected entries. if the Directory exists then it move
    // everything there. else, it makes the Directory.
    char *err = NULL;
    BrowseEntry *list_to_be_moved = E->selected;
    BrowseEntry dest = E->fb->entries[E->fb->cur_row];
    // Move to highlted entry.
    if (!E->selected_size) {
      sprintf(E->notification_buffer, "fb_mv: None was selected to move");
      return;
    }
    for (size_t i = 0; i < E->selected_size; ++i) {
      err = execute_fbsys_command(MOVE, list_to_be_moved[i], dest);
      if (err) {
        sprintf(E->notification_buffer, "fb_mv: %s", err);
        break;
      }
    }
    if (!err) {
      // NOTE: Since no error was encounterred, we unselect everyting.
      for (size_t i = 0; i < E->fb->size; i++)
        E->fb->entries[i].selected = false;
      free(E->selected);
      E->selected = NULL;
      E->selected_size = 0;
      E->selected_cap = 0;
      marking_mode = false;
    }
  } break;
  case SHIFT('c'):
  case 'c': {
    // TODO: the copy function does the same thing as the move
    // the difference is that it does not,
    char *err = NULL;
    BrowseEntry *list_to_be_coppied = E->selected;
    BrowseEntry dest = E->fb->entries[E->fb->cur_row];
    // Move to highlted entry.
    if (!E->selected_size) {
      sprintf(E->notification_buffer, "fb_cp: None was selected to copy");
      return;
    }
    for (size_t i = 0; i < E->selected_size; ++i) {
      err = execute_fbsys_command(COPY, list_to_be_coppied[i], dest);
      if (err) {
        sprintf(E->notification_buffer, "fb_cp: %s", err);
        break;
      }
    }
    if (!err) {
      // NOTE: Since no error was encounterred, we unselect everyting.
      for (size_t i = 0; i < E->fb->size; i++)
        E->fb->entries[i].selected = false;
      free(E->selected);
      E->selected = NULL;
      E->selected_size = 0;
      E->selected_cap = 0;
      marking_mode = false;
    }
  } break;
  case 'a': {
    // Make nameBuff and pass it to fb_append.
    curs_set(1);
    int y = E->renderer->win_h - 2;
    sprintf(label, "%s", "> Create File/Directory: ");
    mvprintw(y, 0, "%s", label);
    Result *res =
        make_prompt_buffer(strlen(label), y, E->renderer->win_w, DRACULA_PAIR);
    switch (res->type) {
    case SUCCESS: {
      char *entry = NULL;
      char *toks[125] = {NULL};
      size_t size = 0;
      for (char *entry = strtok(res->data, " \t"); entry;
           (entry = strtok(NULL, " \t")))
        toks[size++] = (char *)xstrdup(entry);
      for (size_t i = 0; i < size; i++) {
        entry = toks[i];
        editor_new_entry(entry, E);
        sprintf(E->notification_buffer, "%s was made", entry);
        free(toks[i]);
      }
    } break;
    case ERROR: {
      sprintf(E->notification_buffer, "%s", res->data);
    } break;
    default: {
      fprintf(stderr, "Unreachable code\n");
      abort();
    }
    }

    free(res->data);
    free(res);
  } break;
  }
}

void editor_command_(MiEditor *E) {
  editorMode mode = E->mode;
  char *label = " cmd > ";
  int y = E->renderer->win_h - 2;
  Result *res = NULL;
  E->mode = COMMAND;

  curs_set(1);
  editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
                        E->notification_buffer);
  mvprintw(y, 0, "%s", label);

  res = make_prompt_buffer(strlen(label), y, E->renderer->win_w, DRACULA_PAIR);
  if (res->type == SUCCESS)
    editor_command_execute(E, res->data, mode);

  free(res->data);
  free(res);

  E->mode = (E->mode == COMMAND) ? mode : E->mode;
  editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
                        E->notification_buffer);
}
