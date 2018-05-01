#include <git2.h>
#include <microhttpd.h>

int main() {
    git_libgit2_init();

    git_libgit2_shutdown();
    return 0;
}
