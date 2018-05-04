#include "confmgr.h"
#include "logging.h"
#include "thread_pool.h"
#include "utils.h"
#include <git2.h>
#include <microhttpd.h>
#include <stdio.h>
#include <unistd.h>

struct p {
    int id;
};

void *worker(void *p) {
    struct p *pa = (struct p *)p;
    sleep(5);
    log_msg("Thread %d\n", pa->id);
    return (void *)NULL;
}

int main(int argc, char *argv[]) {
    if (argc == 0) {
        printf("Usage : ./mirror configfile");
        return 1;
    }
    (void)argv;
    init_logging();

    struct thread_pool *pool = create_thread_pool(4);

    for (int i = 0; i < 10; i++) {
        struct p *pa = malloc(sizeof(struct p));
        pa->id = i;
        log_msg("Waiting to create thread %d\n", i);
        launch_thread(pool, worker, (void *)pa);
    }
    join_all(pool);
    struct p *pa = malloc(sizeof(struct p));
    pa->id = 19;
    launch_thread(pool, worker, (void *)pa);
    join_all(pool);
    /* git_libgit2_init(); */

    /* struct confmgr *a = read_config(argv[1]); */
    /* apply_conf(a); */
    /* free_confmgr(a); */

    /* git_libgit2_shutdown(); */

    return 0;
}
