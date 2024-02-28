#include <mi.h>

// Abstract part of the clip api.
void *clipboard(ClipBoardEvent e, char *data)
{
    static char *clipboard = NULL;    
    switch (e) {
        case CFREE: { 
            free(clipboard); 
        } break;
        case CGET: {
            return (void *)clipboard;
        } break;
        case CSET: {
            if (clipboard != NULL) free(clipboard);
            int sz = strlen(data);
            clipboard = malloc(sz + 1);
            clipboard = memcpy(clipboard, data, sz);
        } break;
        default: {};
    }
    return NULL;
}
