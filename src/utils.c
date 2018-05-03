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
 * \file utils.c
 * \brief Implementation of utils.h
 */

#include "utils.h"
#include "url_parser.h"

#include <string.h>
#include <stdlib.h>

#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

/**
 * \brief Check that a url is valid, and write its components to the parameters
 * \return 0 if the URL is valid
 */
int match_uri(char *uri, char **scheme, char **host, char **path) {
    struct url_parser u;
    url_parser_init(&u);
    int r;
    if((r = url_parser_parse_url(uri, strlen(uri), 0, &u)) == 0){
        *scheme = url_parser_get_field(&u, UF_SCHEMA, uri);
        *host = url_parser_get_field(&u, UF_HOST, uri);
        *path = url_parser_get_field(&u, UF_PATH, uri);
    }
    return r;
}

bool dir_exists(char *path){
        DIR *d = opendir(path);
        bool flag = false;
        if(d){
                flag = true;
                closedir(d);
        }
        return flag;
}
