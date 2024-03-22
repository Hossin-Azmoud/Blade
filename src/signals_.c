#include <mi.h>
	
void init_signals() 
{
    
    // Disable Ctrl-C.
    signal(SIGINT, SIG_IGN);
    
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
