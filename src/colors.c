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
