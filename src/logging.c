/*
MIT License

Copyright (c) 2018 Nicolas Manichon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/**
 * \file logging.c
 * \brief Implementation of logging.h
 */
#include "logging.h"
#include "../config.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_PTHREAD

#include <pthread.h>

pthread_mutex_t lock;

#endif

time_t time_start;

/**
 * \brief Init the logging library. Sets the program start time
 */
void init_logging(void) {
#ifdef HAVE_PTHREAD

    pthread_mutex_init(&lock, NULL);

#endif

    time(&time_start);
    printf(
        "           file:line|    date      time   |       up time        |> "
        "Message\n");
}

void elapsed(int *years, int *days, int *hours, int *minutes, int *seconds) {
    time_t time_now;
    time(&time_now);
    int diff = (int)difftime(time_now, time_start);
    *seconds = diff % 60;
    diff /= 60;
    *minutes = diff % 60;
    diff /= 60;
    *hours = diff % 24;
    diff /= 24;
    *days = diff % 365;
    diff /= 365;
    *years = diff;
}

/**
 * \brief Log a message to stdout with additional info
 */
void log_msg_private(char *file, int line, char *format, ...) {
    va_list params;
    va_start(params, format);
    time_t t;
    time(&t);
    struct tm *bt;

    bt = localtime(&t);

    int years, days, hours, minutes, seconds;
    elapsed(&years, &days, &hours, &minutes, &seconds);

#ifdef HAVE_PTHREAD

    pthread_mutex_lock(&lock);

#endif

    file = strrchr(file, '/') + 1;

    printf("%15s:%-4d| %04d/%02d/%02d %02d:%02d:%02d | "
           "%02dy-%03dd-%02dh-%02dm-%02ds |> ",
           file, line, bt->tm_year + 1900, bt->tm_mon + 1, bt->tm_mday,
           bt->tm_hour, bt->tm_min, bt->tm_sec, years, days, hours, minutes,
           seconds);
    vprintf(format, params);
#ifdef HAVE_PTHREAD

    pthread_mutex_unlock(&lock);

#endif
    va_end(params);
}
