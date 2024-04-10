#include <mi.h>

void do_resize(int resize_signal)
{
    system("reset");
    fprintf(stderr, "[MI] THE RESIZE FUNCTIONALITY IS NOT IMPLEMENTED YET. n = %i\n", resize_signal);
    exit(1);
}

void init_signals() 
{
    
    // Disable Ctrl-C.
    signal(SIGINT, SIG_IGN); 
    
    // Configure a resize function
    signal(SIGWINCH, do_resize);   

    // enable Ctrl-Z. Ctrl-S
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    term.c_cc[VSTOP] = _POSIX_VDISABLE;
    term.c_cc[VSUSP] = _POSIX_VDISABLE;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

