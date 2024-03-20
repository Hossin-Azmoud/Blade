#include <mi.h>
#include <locale.h>

#define T 0

// #define DEBUG
int editor(char **argv);
int test(char *ent);

int main(int argc, char **argv) {
    (void) argc; 
    if (T) {
        if (argc > 1) {
            return test(argv[1]);
        }
        return 0;
    }
    int ret = editor(argv);
    CLIPBOARD_FREE();
    return ret;
}

int test(char *ent) 
{
    /***
        struct stat {
           dev_t      st_dev;      ID of device containing file
           ino_t      st_ino;      Inode number
           mode_t     st_mode;     File type and mode 
           nlink_t    st_nlink;    Number of hard links
           uid_t      st_uid;      User ID of owner
           gid_t      st_gid;      Group ID of owner
           dev_t      st_rdev;     Device ID (if special file)
           off_t      st_size;     Total size, in bytes
           blksize_t  st_blksize;  Block size for filesystem I/O
           blkcnt_t   st_blocks;   Number of 512 B blocks allocated
           struct timespec  st_atim;
           struct timespec  st_mtim;
           struct timespec  st_ctim;

       #define st_atime  st_atim.tv_sec
       #define st_mtime  st_mtim.tv_sec
       #define st_ctime  st_ctim.tv_sec
       };
    ***/ 
	struct stat info;
	int status = stat(ent, &info);
    if (!status) {
        printf("INFO ABOUT %s\n", ent);
        printf("size %zu\n", info.st_size);
        printf("blocksz %zu\n", info.st_blksize);
        printf("blocks %zu\n", info.st_blocks);
        printf("Inode %zu\n", info.st_ino);
        printf("Links %zu\n", info.st_nlink);
    } else {
        printf("Failed to load dir info.\n");
    }    
    return 0;
}

int editor(char **argv)
{
    
    setlocale(LC_CTYPE,""); 
    open_logger();
    emoji_init(); // Init the emojis cache.
    MiEditor *E = init_editor(argv[1]);
    int c = 0;
    if (E == NULL) {
        goto EXIT_AND_RELEASE_RESOURCES;
    }
    
    while ((c = getch()) != EOF && !E->exit_pressed) {
        editor_load_layout(E);

        if (is_move(c)) {
            handle_move(c, E);
            goto UPDATE_EDITOR;
        }
        
        switch (E->fb->type) 
        {
            case DIR__: {
                fb_update(c, E);    
            } break;
            case FILE__:
            case NOT_EXIST: {
                editor_update(c, E);
            } break;
            default: {} break;
        }
    UPDATE_EDITOR:
        editor_render(E);
        if (E->exit_pressed) break;
    }

EXIT_AND_RELEASE_RESOURCES:
    endwin();
    release_editor(E);
    close_logger();
    return 0;
}

