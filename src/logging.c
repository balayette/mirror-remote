#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void log_msg(char *format, ...) {
    va_list params;
    va_start(params, format);
    time_t t;
    time(&t);
    struct tm *bt;

    bt = localtime(&t);

    printf("%d/%d/%d %d:%d:%d |> ", bt->tm_year + 1900, bt->tm_mon + 1,
           bt->tm_mday, bt->tm_hour, bt->tm_min, bt->tm_sec);
    vprintf(format, params);
    va_end(params);
}
