#include <fcntl.h>
#include <blade.h>
#include <linux/limits.h>
// #include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const char *defaul_cfg = "\
autosave:     1\n\
indent_char:  \" \"\n\
indent_count: 6\n\
# Theme.\n\
background:   0x191919\n\
foreground:   0xffccbb\n\
keyword_color: 0xf9ca24\n\
type_color: 0xff7675\n\
funcall_color: 0x00ffff\n\
special_token_color: 0x01a3a4\n\
string_lit_color: 0x6ab04c\n\
comment_color: 0x3f553c\
";

EditorConfig_t *load_editor_config(char *file) {
  EditorConfig_t *cfg;
  bool stx_err = false;
  BladeTokenType sep_kind;
  char *lhs, *rhs;
  int stream;
  Line *line;
  TokenList *tokens;
  int i = 0;
  if (!file)
    return NULL;

  cfg = malloc(sizeof(*cfg));
  memset(cfg, 0, sizeof(*cfg));
  cfg->cfg_path = string_dup(file);
  stream = open(cfg->cfg_path, O_RDONLY);
  if (stream < 0) {
    free(cfg);
    return (NULL);
  }
 
  line = Alloc_line_node(0);
  free(line->content);
  for (char *next = read_next_line(stream); next;
       (next = read_next_line(stream)), i++) {
    line->content = next;
    line->size    = strlen(next);
    retokenize_line(line, UNSUP);
    tokens = &line->token_list;

    rhs = tokens->_list[0].data;
    if (tokens->_list[0].kind == HASHTAG || !*rhs)
      continue;
    sep_kind = tokens->_list[1].kind;
    if (sep_kind != COLON && sep_kind!= EQ) {
      stx_err = true;
      fprintf(stderr, "INVALID SYNTAX: there should have been `=` or `:` after %s token\n", rhs);
      break;
    }
    lhs = tokens->_list[2].data;
    if (strcmp(rhs, "autosave") == 0) {
      cfg->autosave = atoi(lhs);
      continue;
    }
    if (strcmp(rhs, "indent_char") == 0) {
      assert(*lhs == '"');
      assert(*(lhs + 1) != 0);
      cfg->indent_char = *(lhs + 1); // The letter directly after the qoutes.
      continue;
    }
    if (strcmp(rhs, "indent_count") == 0) {
      cfg->indent_count = atoi(lhs);
      continue;
    }
    if (strcmp(rhs, "background") == 0) {
      cfg->theme.background = (int)np_atoi_base(lhs, NULL);
      continue;
    }

    if (strcmp(rhs, "foreground") == 0) {
      cfg->theme.foreground = (int)np_atoi_base(lhs, NULL);
      continue;
    }

    if (strcmp(rhs, "keyword_color") == 0) {
      cfg->theme.keyword_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
    if (strcmp(rhs, "type_color") == 0) {
      cfg->theme.type_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
    if (strcmp(rhs, "funcall_color") == 0) {
      cfg->theme.funcall_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
    if (strcmp(rhs, "special_token_color") == 0) {
      cfg->theme.special_token_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
    if (strcmp(rhs, "string_lit_color") == 0) {
      cfg->theme.string_lit_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
    if (strcmp(rhs, "comment_color") == 0) {
      cfg->theme.comment_color = (int)np_atoi_base(lhs, NULL);
      continue;
    }
  }

  close(stream);
  free((line->token_list)._list);
  free(line);
  if (!stx_err)
    return (cfg);
  free(cfg->cfg_path);
  free(cfg);
  return NULL;
}


static bool write_default_cfg(char *xdg_cfg_path)
{
  FILE *fp = fopen(xdg_cfg_path, "w");
  if (fp == NULL) {
    fprintf(stderr, "Could not write to %s\n", xdg_cfg_path);
    return false;
  }
  fwrite(defaul_cfg , 1, strlen(defaul_cfg), fp);
  fclose(fp);
  return true;
}

EditorConfig_t *editor_resolve_cfg(const char *cfg_path) 
{
  // TODO: Check for given path.
  
  char *value = getenv("HOME");
  char xdg[PATH_MAX];
  char xdg_cfg_path[PATH_MAX];
  char *xdg_cfg_dir = malloc(PATH_MAX);
  EditorConfig_t *cfg = NULL;
  // Path *p;
  memset(xdg_cfg_path, 0x0, PATH_MAX);

  if (cfg_path) {
    cfg  = load_editor_config((char *)cfg_path);
    return cfg;
  }

  // TODO: Check for the existence of xdg_cfg_path. 
  sprintf(xdg, "%s/.config", value);
  sprintf(xdg_cfg_dir, "%s/.config/blade", value);
  sprintf(xdg_cfg_path, "%s/blade.cfg", xdg_cfg_dir);
  // p = path_alloc(32);
  if (get_entry_type(xdg) == NOT_EXIST)
      make_dir(xdg);
  if (get_entry_type(xdg_cfg_dir) == NOT_EXIST)
    make_dir(xdg_cfg_dir);
  if (get_entry_type(xdg_cfg_path) == NOT_EXIST) {
    if (!write_default_cfg(xdg_cfg_path))
      return cfg;
  }

  // TODO: Load the config.
  return (load_editor_config(xdg_cfg_path)); 
}
