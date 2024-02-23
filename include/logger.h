#ifndef LOGGER_H___
#define LOGGER_H___
#include <stdio.h>
#include <stdlib.h>

#define LOGGER_FILE "Logs.mi.txt"

void open_logger();
void close_logger();
FILE *get_logger_file_ptr();
#endif // !LOGGER_H___
