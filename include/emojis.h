#ifndef EMOJIS_H
#define EMOJIS_H
#define EMOJI_CAP 25

typedef enum EmojiKind {
    E_FOLDER = 0,
    E_FILE,
    E_C_FILE,
    E_COUNT
} EmojiKind;

typedef enum EmojiPoolAction {
    INIT,
    GET
} EmojiPoolAction;

char *emoji_pool(EmojiPoolAction a, EmojiKind kind);

#define emoji_init() emoji_pool(INIT, 0) 
#define emoji_get(e) emoji_pool(GET, e)

#endif // EMOJIS_H
