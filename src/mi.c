#include <mi.h>

size_t digit_len(size_t n)
{
	size_t length = 0;

	while (n > 0) {
		/* if I want more info (n % base + '0') */
		n /= 10;
		length++;
	}
	
	return (length);
}
