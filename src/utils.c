#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "joylent/http_parser.h"

void match_uri(char *uri, char **scheme, char **authority, char **path) {
    (void)scheme;
    (void)authority;
    (void)path;
    struct http_parser_url *u = malloc(sizeof(struct http_parser_url));
    http_parser_url_init(u);
    http_parser_parse_url(uri, strlen(uri), 0, u);
    printf("%s\n",http_parser_url_schema(u, uri));
}
