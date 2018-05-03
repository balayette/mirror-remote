#include "confmgr.h"
#include <git2.h>
#include <microhttpd.h>
#include <stdio.h>
#include "utils.h"
#include "logging.h"

int main(int argc, char *argv[]) {
    if (argc == 0) {
        printf("Usage : ./mirror configfile");
        return 1;
    }
    init_logging();
    git_libgit2_init();

    struct confmgr *a = read_config(argv[1]);
    apply_conf(a);
    free_confmgr(a);

    git_libgit2_shutdown();

    return 0;
}
