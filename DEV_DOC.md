# Dev Ideas.
- There is an idea that I have about syntax highlighting which is that it needs to be more flexable.
- Each language has three things.

- Keywords: A list of Keywords that should be viewed differently than other words.
- Types:    A Set of rule that define how types should be tokenize and recognized.
    - In C types are recognized using these rules:
        - A type is something that comes before * and is an ID token.
        EX: int *x = <>
        - A types is something that comes before a function declaration
        EX: int void *func() {  }
        - A type is and ID that comes before an ID.

        So it can be arranged this way:
            [TYPE, *] [TYPE, FDECLARATION] [TYPE(ID), ID] 

    - In python it is even much simpler:
        - A type is something that comes after `:`
        EX: x: int
        - A type is something that comes after `->`

        So it can be arranged this way:
            [ID, :, TYPE] [->, TYPE]

- SO BASICALLY, What I want to do is an engine that can parse these rules and apply them inside the editor.
the rule will be syntactically this way:
    a .mi file will have this config
    .py#keywords: A vector of words that can be applied as keywords of a .py file.
    .c#keywords:  A vector of words that can be applied as keywords of a .c file.
    .x#keywords:  A vector of words that can be applied as keywords of a .x file.
    .py#types:    [ID, :, TYPE] [->, TYPE]
    .c#types:     [TYPE, *] [TYPE, FDECLARATION] [TYPE(ID), ID]
    .c#special:   #$ enum #include struct typedef union
    
