#include "gitmgr.h"
#include <string.h>
#include "confmgr.h"
#include <git2.h>
#include <stdbool.h>
#include "logging.h"

int check_err(int r){
        if(r < 0)
                log_msg("%s\n", giterr_last()->message);
        return r;
}

int init_repo(char *path, git_repository **repo){
        return git_repository_init(repo, path, false);
}

int clone_repo(char *url, char *path, git_repository **repo){
        log_msg("Cloning %s to %s\n", url, path);
        git_clone_options opts;
        git_clone_init_options(&opts, GIT_CLONE_OPTIONS_VERSION);
        opts.bare = 1;
        return check_err(git_clone(repo, url, path, &opts));
}

void clone_all(struct confmgr *c){
        git_repository *r = NULL;
        char *path = calloc(strlen(c->store) + 1, sizeof(char));
        for(size_t i = 0; i < c->repo_count; i++){
                clone_repo(c->repo_urls[i], path, &r);
                git_repository_free(r);
        }
}
