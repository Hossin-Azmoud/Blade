#include <mi.h>

void colorize(int y, int xs, int xe, int pair) {
    mvchgat(y, xs, xe, A_NORMAL, pair, NULL);
}

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
        (color >> 8 ) & 0xFF, 
        (color >> 0 ) & 0xFF
    );
}

void init_colors(void) 
{
    start_color();
    make_new_color_u32(COLOR_DRACULA,      0x001900);
    make_new_color_u32(COLOR_LIGHT_YELLOW, 0xfdcb6e);
    make_new_color_u32(CHI_GONG_RED,       0xff7675);
    make_new_color_u32(COLOR_SILVER_,      0x4bcffa);
    make_new_color_u32(COLOR_GREY_,        0x000000);
    make_new_color_u32(COLOR_GREY_LIGHT,   0xFFEDD8);
    make_new_color_u32(COLOR_YELLOW_,      0xf9ca24);
    make_new_color_u32(COLOR_APPLE,        0x6ab04c);
    make_new_color_u32(COLOR_COMMENT,      0x3f553c); 
    make_new_color_u32(COLOR_TAG,          0xe74c3c);
    make_new_color_u32(COLOR_BRIGHT_GREEN, 0x2ecc71);
    make_new_color_u32(COLOR_SPECIAL_TOKENS_COLOR, 0x01a3a4);
    make_new_color_u32(COLOR_BRIGHT_YELLOW, 0xf39c12);
    make_new_color(COLOR_BLUE, 0, 44, 84); 
    make_new_color(COLOR_RED, 210, 31, 60); 
    

    // pair_number, foreground, background
    init_pair(DRACULA_PAIR, COLOR_WHITE, COLOR_DRACULA);
    init_pair(CHI_GONG_RED_PAIR, CHI_GONG_RED, COLOR_GREY_);
    init_pair(LIGHT_YELLOW_PAIR, COLOR_LIGHT_YELLOW, COLOR_GREY_);
    init_pair(HIGHLIGHT_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(SPECIAL_TOKEN, COLOR_SPECIAL_TOKENS_COLOR, COLOR_GREY_);
    init_pair(BRIGHT_GREEN_PAIR, COLOR_BRIGHT_GREEN, COLOR_GREY_);
    init_pair(BRIGHT_YELLOW_PAIR, COLOR_BRIGHT_YELLOW, COLOR_GREY_);
    init_pair(TAG_PAIR, COLOR_TAG, COLOR_GREY_);
    init_pair(MAIN_THEME_PAIR, COLOR_WHITE, COLOR_GREY_);
    init_pair(HIGHLIGHT_THEME, COLOR_BLACK, COLOR_GREY_LIGHT);
    init_pair(SECONDARY_THEME_PAIR, COLOR_GREY_, COLOR_YELLOW_);
    init_pair(ERROR_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(BLUE_PAIR, COLOR_WHITE, COLOR_BLUE);
    init_pair(YELLOW_PAIR, COLOR_YELLOW_, COLOR_GREY_);
    init_pair(CALL_SYNTAX_PAIR, COLOR_CYAN, COLOR_GREY_);
    init_pair(STRING_LIT_PAIR, COLOR_APPLE, COLOR_GREY_);
    init_pair(COMENT_PAIR, COLOR_COMMENT, COLOR_GREY_);
    init_pair(NUM_PAIR, COLOR_SILVER_, COLOR_GREY_);
    
}

