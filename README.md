# Mi
![EDITOR-UI](./img/V0.png)
Tui text editor made in ncurses

# Quick start

## Install ncurses using apt
```console
    $ sudo apt-get install libncurses5-dev libncursesw5-dev
```
## Compile

```console
    $ make
```

## Run
```console
    $ cd bin
    $ ./mi <file_path>
```

## Keys
- To exit and save the file press `F1`
- To save without exiting the editor press `w` while u r in a `NORMAL` mode
- To switch to `NORMAL` mode press `ESCAPE`
- To switch to `VISUAL mode press `v` while u r in `NORMAL` mode
- To switch to `INSERT` mode press `i` while u r in `NORMAL` mode
- To copy data around u press on the vim copy key `y` twise then if u want to paste the data u just press `p`
- To exit press `F2` (file wont be saved tho!)

## Implemented Syntax Highlighting
(*) Python
(*) Js
()  C
()  Rust
()  Go



