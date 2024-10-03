#include <common.h>
#include <blade.h>
#include <stdio.h>
int editor(char **argv, const char *cfg_path) {
  setlocale(LC_CTYPE, "");  
  // printf("[CFG LOADING]\n");
  EditorConfig_t *cfg = editor_resolve_cfg(cfg_path);
  if (!cfg)
    return 1;
  else {
    // printf("CFG was loaded with success. PATH: %s\n", cfg->cfg_path);
    // printf("summary:\n");
    // printf("-------Parsed configuration------\n");
    // printf("{\n");
    // printf("  autosave: %s, \n", cfg->autosave ? "true" : "false");
    // printf("  indent_char: %c, \n", cfg->indent_char);
    // printf("  indent_count: %d, \n", cfg->indent_count);
    // printf("  back: %x, \n", cfg->theme.background);
    // printf("  front: %x, \n", cfg->theme.foreground);
    // printf("  keyword_color: %x, \n", cfg->theme.keyword_color);
    // printf("  type_color: %x, \n", cfg->theme.type_color);
    // printf("  funcall_color: %x, \n", cfg->theme.funcall_color);
    // printf("  special_token_color: %x, \n", cfg->theme.special_token_color);
    // printf("  string_lit_color: %x, \n", cfg->theme.string_lit_color);
    // printf("  comment_color: %x, \n", cfg->theme.comment_color);
    // printf("}\n");
  }

  emoji_init(); // Init the emojis cache.
  
  BladeEditor *E = init_editor(argv[1], cfg);
  // resolve_path(char *src, char *dest)

  int c = 0;
  if (E == NULL) {
    goto EXIT_AND_RELEASE_RESOURCES;
  }

  while (true) {

    // NOTE: Dont remove halfdelay because it is essencial for some signal
    // interrupts, sometimes u need to wait for something to happen before
    // updating.
    // halfdelay(0.100); // NOTE: wait ((1000ms/10ms) * n) => (1000ms/10ms) *
    // 1ms => 100ms.
    
    c = getch();
    if (c == KEY_RESIZE) {
      editor_load_layout(E);
      // re-init the file browser..
      if (E->fb->type == DIR__)
        reinit_fb_bounds(E->fb, E->renderer->win_h);

      sprintf(E->notification_buffer, "resize detected!");
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

    switch (E->fb->type) {
    case DIR__: {
      editor_file_browser(c, E);
    } break;
    case FILE__:
    case NOT_EXIST: {
      editor_update(c, E);
    } break;
    default: {
    } break;
    }

  UPDATE_EDITOR:
    // sprintf(E->notification_buffer, "$END: %d $CUR: %d", (int)E->fb->end, (int)E->fb->cur_row);
    editor_render(E);
    if (E->exit_pressed)
      break;
  }

EXIT_AND_RELEASE_RESOURCES:
  release_editor(E);
  // close_logger();
  return 0;
}
