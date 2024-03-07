#include <mi.h>

void make_new_color(int index, int r, int g, int b)
{
    if (index > 255) return;
    init_color(index, 
        ((r * 1000) / 255), 
        ((g * 1000) / 255), 
        ((b * 1000) / 255)
    );
}

void make_new_color_u32(int index, uint32_t color)
{
    make_new_color(index, 
        (color >> 16) & 0xFF, 
        (color >> 8) & 0xFF, 
        (color >> 0) & 0xFF
    );
}

void init_colors() 
{
    start_color();
    // pair_number, foreground, background
    make_new_color_u32(COLOR_SILVER_,  0x4bcffa);
    make_new_color_u32(COLOR_GREY_,    0x04081a);
    make_new_color_u32(COLOR_YELLOW_,  0xf9ca24);
    make_new_color_u32(COLOR_APPLE,    0x6ab04c);
    make_new_color_u32(COLOR_COMMENT,  0x212e1f); 
    make_new_color(COLOR_BLUE, 0, 44, 84); 
    make_new_color(COLOR_RED, 210, 31, 60); 

    init_pair(MAIN_THEME_PAIR, COLOR_WHITE, COLOR_GREY_);
    init_pair(HIGHLIGHT_THEME, COLOR_GREY_, COLOR_WHITE);
    init_pair(SECONDARY_THEME_PAIR, COLOR_GREY_, COLOR_YELLOW_);
    init_pair(ERROR_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(BLUE_PAIR, COLOR_WHITE, COLOR_BLUE);
    init_pair(KEYWORD_SYNTAX_PAIR, COLOR_YELLOW_, COLOR_GREY_);
    init_pair(CALL_SYNTAX_PAIR, COLOR_CYAN, COLOR_GREY_);
    init_pair(STRING_LIT_PAIR, COLOR_APPLE, COLOR_GREY_);
    init_pair(COMENT_PAIR, COLOR_COMMENT, COLOR_GREY_);
    init_pair(NUM_PAIR, COLOR_SILVER_, COLOR_GREY_);
}

