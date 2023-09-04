#ifndef TUI_H
#define TUI_H

#include <stdio.h>

#define STD_NRM      "\x1B[0m"
#define STD_RED      "\x1B[1;31m"
#define STD_GREEN    "\x1B[1;32m"
#define STD_YELLOW   "\x1B[1;33m"
#define STD_BLUE     "\x1B[1;34m"
#define STD_MAGINTA  "\x1B[1;35m"
#define STD_CYAN     "\x1B[1;36m"
#define STD_WHITE    "\x1B[1;37m"
#define reset_term_color() puts(STD_NRM);

#if 0
	#define TC_B_NRM  "\x1B[0m"
	#define TC_B_RED  "\x1B[0;31m"
	#define TC_B_GRN  "\x1B[0;32m"
	#define TC_B_YEL  "\x1B[0;33m"
	#define TC_B_BLU  "\x1B[0;34m"
	#define TC_B_MAG  "\x1B[0;35m"
	#define TC_B_CYN  "\x1B[0;36m"
	#define TC_B_WHT  "\x1B[0;37m"

	#define TC_BG_NRM "\x1B[40m"
	#define TC_BG_RED " \x1B[41m"
	#define TC_BG_GRN "\x1B[42m"
	#define TC_BG_YEL "\x1B[43m"
	#define TC_BG_BLU "\x1B[44m"
	#define TC_BG_MAG "\x1B[45m"
	#define TC_BG_CYN "\x1B[46m"
	#define TC_BG_WHT "\x1B[47m"
#endif
#endif // TUI_H
