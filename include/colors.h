#ifndef COLORS_H
#define COLORS_H

// COLOR INDECES
#define COLOR_GREY_           8
#define COLOR_YELLOW_         9
#define COLOR_APPLE          10
#define COLOR_COMMENT        11
#define COLOR_SILVER_        12
#define COLOR_TAG            13
#define COLOR_BRIGHT_GREEN   14
#define COLOR_BRIGHT_YELLOW  15
#define COLOR_GREY_LIGHT     16
#define COLOR_SPECIAL_TOKENS_COLOR 17
#define COLOR_LIGHT_YELLOW 18
#define CHI_GONG_RED 19

// PAIRS INDECES
#define SECONDARY_THEME_PAIR 1
#define MAIN_THEME_PAIR      2
#define ERROR_PAIR           3
#define BLUE_PAIR            4
#define HIGHLIGHT_THEME      5 
#define YELLOW_PAIR          6
#define STRING_LIT_PAIR      7
#define CALL_SYNTAX_PAIR     8
#define COMENT_PAIR          9
#define NUM_PAIR             10
#define TAG_PAIR             11
#define BRIGHT_GREEN_PAIR    12
#define BRIGHT_YELLOW_PAIR   13
#define HIGHLIGHT_WHITE      14
#define SPECIAL_TOKEN        15
#define LIGHT_YELLOW_PAIR    16
#define CHI_GONG_RED_PAIR    17 

void make_new_color(int index, int r, int g, int b);
void make_new_color_u32(int index, uint32_t color);
void init_colors();
void colorize(int y, int xs, int xe, int pair);

#endif // COLORS_H
