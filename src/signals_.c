#include <mi.h>

static void do_resize(int resize_signal)
{
    (void) resize_signal;    
    // MiEditor *E = editor_get();
    system("reset");
    printf("Hello the term was resized!\n");
    // exit(1);
}

static void init_ctrl_signals() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    term.c_cc[VSTOP] = _POSIX_VDISABLE;
    term.c_cc[VSUSP] = _POSIX_VDISABLE;

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
static void init_resize_sig() {
    struct sigaction sa, priorsa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = do_resize;
    sigaction(SIGWINCH, &sa, &priorsa);
}

void init_signals() 
{
    
    // Disable Ctrl-C.
    signal(SIGINT, SIG_IGN); 
    // enable Ctrl-Z. Ctrl-S
    init_ctrl_signals();
    // Configure a resize function
    // signal(SIGWINCH, do_resize);   
    init_resize_sig();
}

