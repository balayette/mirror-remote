#include <microhttpd.h>
#include <git2.h>
#include "mirror.h"

int main(){
    git_libgit2_init();

    git_libgit2_shutdown();
    return 0;
}
