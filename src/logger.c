#include <logger.h>
static FILE *stdl = NULL;

void open_logger()
{
    stdl = fopen(LOGGER_FILE, "a+");
}

FILE *get_logger_file_ptr()
{
    return stdl;
}

void close_logger()
{
    if (stdl) fclose(stdl);
}
