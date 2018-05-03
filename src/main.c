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

    struct confmgr *a = read_config("config");
    apply_conf(a);
    free_confmgr(a);

    git_libgit2_shutdown();

    return 0;
}
