#include <mi.h>

char *script_type_as_str(ScriptType s) 
{
    switch (s) {
        case PYTHON: return "PYTHON";
        case C: return "C";
        case JS: return "JAVASCRIPT";
        default: return "UNSUPPORTED";
    }    
    
    return "UNSUPPORTED";
}

KeywordList *get_keywords_list(ScriptType s)
{
    static KeywordList list[UNSUP] = {
        [PYTHON] = {
            .size=23,
            ._list={
                "def", 
                "for", 
                "if",
                "self",
                "while", 
                "else", 
                "elif", 
                "True", 
                "False", 
                "max",
                "min",
                "match",
                "class",
                "str",
                "range",
                "in",
                "while",
                "import",
                "from",
                "lambda",
                "break",
                "continue",
                "yield"
            },
        },
        [C] = {
            .size=24,
            ._list={
                "void", 
                "int", 
                "char",
                "struct",
                "while", 
                "else", 
                "if", 
                "true", 
                "false", 
                "switch",
                "typedef",
                "union",
                "long",
                "short",
                "float",
                "size_t",
                "return",
                "break",
                "continue",
                "goto",
                "double",
                "uint8_t",
                "uint16_t",
                "uint32_t"
            },
        },
        
        [JS] = {
            .size=56,
            ._list={
                "abstract",
                "arguments",
                "await", 	
                "boolean"
                "case", 	
                "catch",
                "break",
                "byte",
                "char", 	
                "class",
                "const",
                "continue",
                "do",
                "debugger",
                "default",
                "do",
 	 	        "delete",
                "else",
                "enum",
                "eval",
                "double",
                "false",
                "finally",
                "export",
                "extends",
         	    "float",
                "for",
                "function",
                "goto", 	
                "if",
                "implements",
                "import",
                "int",
                "long",
                "private",
                "interface",
                "let",
                "new",
                "package",
                "instanceof",
                "native",
                "static",
                "short",
                "return",
                "in",
                "null",
                "throw",
                "throws",
                "protected",
                "typeof",
                "with",
                "try",
                "public",
                "void",
                "true",
                "yield",
                "super"
            },
        }
    };

    if (s < UNSUP) return (list + s);
    return NULL;
}

ScriptType get_script_type(char *spath)
{
    int it = strlen(spath) - 1;
    char extension[32] = { 0 };
    while (it >= 0) {
        if (spath[it] == '.') {
            strcpy(extension, (spath + it));
            break;
        }

        it--;
    }
    
    if (*extension == 0)return UNSUP;
    
    if (!strcmp(extension, ".py")) return PYTHON;
    if (!strcmp(extension, ".js")) return JS;
    if (!strcmp(extension, ".c"))  return C;

    return UNSUP;
}
