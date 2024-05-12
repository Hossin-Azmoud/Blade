#include <mi.h>
#define INIT_SCRIPT_PATH "init.mi"

// TODO: Implement -v/--version flags and --help
void editor_help(char *program) {
    printf("\nusage => %s <file/directory>\n", program);
    printf("flags:\n");
    printf("-h, --help    => Prints the help message into stdout.\n");
    printf("-v, --version => Prints the version of the editor.\n\n");
}

bool check_args(int argc, char **argv) 
{
    char *program = *argv;
    if (argc >= 2) {
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
            printf(MI_V);
            return true;
        }
        
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            editor_help(program); 
            return true;
        }
    }

    return false;
}
#define EXP
void some_function();
void some_function2();
int main(int argc, char **argv) {
    (void) argc; 
    int ret = 0;

#ifdef EXP
    (void) argv;
    some_function2(".");

#else
    if (!check_args(argc, argv)) {
        ret = editor(argv);
        CLIPBOARD_FREE();
    }
#endif /* ifdef EXP */
    return ret;
}

void some_function() {
    int w, h, mx, my, c = 0, x = 0, y = 0;

    WINDOW *win = initscr();

    raw();
    keypad(stdscr, TRUE);
    noecho();
    getmaxyx(win, h, w);
    getbegyx(win,my,mx);

	while (c != 'q')
    {
        mvprintw(y, x, "[%s] w: %i, h: %i, minx: %i miny: %i\n", 
                 getenv("TERM"), w, h, mx, my);
        getmaxyx(win, h, w);
        getbegyx(win,my,mx);
        c = getch();
        if (c == KEY_RESIZE) {
			mvprintw(y++, x, "Resize.\n");
		}
		y++;
    }

    endwin();
}


char *join_path(char *left, char *right) {

	int ls = strlen(left), rs = strlen(right);
	char *joined = malloc(ls + rs + 2);
	memset(joined, 0, ls + rs + 2);
	joined = strcpy(joined, left);

	if (left[ls - 1] != '/')  {
		joined = strcat(joined, "/");
	}

	joined = strcat(joined, right);
	return (joined);
}

#define FILE_SYSTEM_CAP 8
typedef struct filessystem_t filessystem_t;
typedef struct filessystem_t {
    BrowseEntryT  etype;       // FILE__, DIR__, NOT_EXIST.
    FileType      ftype;       // C, PYTHON, JS...
	size_t        ifs_size;        // Size of the file or the directory...
	size_t        ifs_cap;        // Size of the file or the directory...
    char          *full_path; // /usr/foo 
    char          *value;     // foo
	filessystem_t *parent; // In case we want to go up the tree.
	filessystem_t **ifs; // if this filesystem_t is a directory. then we need to store the files and directories it holds here.
} filessystem_t;


/*
 			(fs)
			 \ifs => (fs)

 */
filessystem_t *slurp_fs(char *path)
{
	filessystem_t *fs = NULL;
	char **entries = NULL;
	char *tmp      = NULL;

	if(path == NULL)
		return (fs);

	fs            = malloc(sizeof(*fs));
	fs->parent    = NULL;
	memset(fs, 0, sizeof(*fs));
	fs->full_path = realpath(path, NULL);
	fs->value     = string_dup(path);
	fs->etype     = get_entry_type(fs->full_path);

	if (fs->etype == FILE__) 
	{
		// fs->ftype = get_file_type(fs->full_path);
		
		return (fs);
	}

	entries     = read_entire_dir(fs->full_path);
	fs->ifs     = malloc(sizeof(fs) * FILE_SYSTEM_CAP);
	fs->ifs_cap = FILE_SYSTEM_CAP;
	
	memset(fs->ifs, 0, FILE_SYSTEM_CAP * sizeof(*(fs->ifs)));

	for (int i = 2; entries[i] != NULL; i++) {	
		// > Resolve the path. [DONE]
		// > if the path is for a directory u call 
		// the current func on it and store the result in fs->ifs[i];
		// > if it is a file then u just add it normally.
		if (fs->ifs_size >= fs->ifs_cap) 
			fs->ifs = realloc(fs->ifs, (fs->ifs_cap) * sizeof(*fs) * 2);
			
		tmp = join_path(fs->full_path, entries[i]);
		
		fs->ifs[i - 2] = slurp_fs(tmp);
		fs->ifs[i - 2]->parent = fs;
		// > retrieve the name of the entry.
		free(fs->ifs[i - 2]->value);
		fs->ifs[i - 2]->value = string_dup(entries[i]);
		fs->ifs_size++;
		free(tmp);
	} 
	
	fs->ifs[fs->ifs_size] = NULL;
	return (fs);
}

void print_fs(filessystem_t *fs, int depth)
{
	// TODO: Make a func that prints a tree of the directories struct.
	if (!fs) return;

	if (fs->parent == NULL)
		printf("ROOT: %s\n", fs->full_path);

	if (fs->etype == FILE__) {
		for (int j = 0; j < depth; ++j)
			printf(" ");

		printf("{ FILE } %s\n", (fs->value));
		return;
	}

	for (int j = 0; j < depth; ++j)
		printf(" ");

	printf("{DIR} %s %zu\n", fs->value, fs->ifs_size);
	if (fs->ifs_size && fs->ifs) {
		for (size_t i = 0; (i < fs->ifs_size) && (fs->ifs[i] != NULL); i++) {
			print_fs(fs->ifs[i], depth + 2);
		}
	}
}

void some_function2(char *path) {
	filessystem_t *fs = slurp_fs(path);
	// > testing would be making an ls functions.
	// > a rm fucntion that deletes a dir tree from the tail to the head.
	if (fs != NULL) {
		print_fs(fs, 1);
	} else {
		printf("There was a problem loading the file system at: %s\n", path);
	}
}

