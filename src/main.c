#include "confmgr.h"
#include <git2.h>
#include <microhttpd.h>
#include <stdio.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc == 0) {
        printf("Usage : ./mirror configfile");
        return 1;
    }
    (void)argv;
    git_libgit2_init();

    char *scheme, *host, *path;
    match_uri("http://github.com:100/d1adev/blockchan", &scheme, &host, &path);

    git_libgit2_shutdown();

    return 0;
}
