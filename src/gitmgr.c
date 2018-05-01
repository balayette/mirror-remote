#include "gitmgr.h"
#include <git2.h>
#include <git2/repository.h>
#include "stdio.h"
#include <git2/common.h>

int check_err(int r){
        if(r < 0)
                printf("%s\n", giterr_last()->message);
        return r;
}

int init_repo(char *path, git_repository **repo){
        return git_repository_init(repo, path, 0);
}

int clone_repo(char *url, char *path, git_repository **repo){
        return check_err(git_clone(repo, url, path, NULL));
}
