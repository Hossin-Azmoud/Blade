#include <logger.h>
static FILE *__Log_file = NULL;

void open_logger()
{
    __Log_file = fopen(LOGGER_FILE, "w+");
}

FILE *get_logger_file_ptr()
{
    return (__Log_file);
}

void close_logger()
{
    fclose(__Log_file);
}
