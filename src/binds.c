#include <mi.h>

void editor_handle_binding(vKeyBindingQueue *bindings)
{
    editor_identify_binding(bindings);
    switch (bindings->kind)
    {
        case COPY_LINE: {} break;
        case DEL_LINE: {} break;
        case NOT_VALID: {} break;
        default: {} break;
    }
}

void editor_identify_binding(vKeyBindingQueue *bindings)
{
    
    if (!bindings->size)
    {
        bindings->kind = NOT_VALID;
        return;
    }

    if (bindings->keys[0] == 'y' && bindings->keys[1] == 'y') { // Copy the current line into clipboard.
        bindings->kind = COPY_LINE;
        return; 
    }

    if (bindings->keys[0] == 'd' && bindings->keys[1] == 'd') { // delete the current line into clipboard.
        bindings->kind = DEL_LINE;
        return; 
    }
}

