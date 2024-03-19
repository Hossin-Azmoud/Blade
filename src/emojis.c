#include <mi.h>

// Emojis.
static uint32_t emojis_[] = {
    [E_FOLDER] = (uint32_t)(L''),
    [E_FILE]   = (uint32_t)(L'󰈚'),
    [E_C_FILE] = (uint32_t)(L'')
};

char *emoji_pool(EmojiPoolAction a, EmojiKind kind)
{
    static char emojis[EMOJI_CAP][5] = {0}; 

    switch (a) {        
        case INIT: {
            decode_utf8(emojis_[E_FOLDER], emojis[E_FOLDER]);
            decode_utf8(emojis_[E_FILE], emojis[E_FILE]);
            decode_utf8(emojis_[E_C_FILE], emojis[E_C_FILE]);
        } break;
        case GET:  {
            if (kind < E_COUNT) {
                return emojis[kind];   
            } else {
                return NULL;
            }
        } break;
        default: {
        } break;
    }

    return NULL;
}
