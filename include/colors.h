#ifndef COLORS_H
#define COLORS_H

#define COLOR_GREY_   8
#define COLOR_YELLOW_ 9
#define COLOR_APPLE 10
#define COLOR_COMMENT 11

#define SECONDARY_THEME_PAIR 1
#define MAIN_THEME_PAIR      2
#define ERROR_PAIR           3
#define BLUE_PAIR            4
#define HIGHLIGHT_THEME      5 
#define KEYWORD_SYNTAX_PAIR  6
#define STRING_LIT_PAIR      7
#define CALL_SYNTAX_PAIR     8
#define COMENT_PAIR          9

void make_new_color(int index, int r, int g, int b);
void make_new_color_u32(int index, uint32_t color);
#endif // COLORS_H
