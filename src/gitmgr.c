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
 * \file gitmgr.c
 * \brief Implementation of gitmgr.h
 */
#include "../config.h"
#include "confmgr.h"
#include "gitmgr.h"
#include "logging.h"
#include "memutils.h"
#include "thread_pool.h"
#include "utils.h"
#include <git2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_PTHREAD

#include <pthread.h>
#include <unistd.h>

#endif

struct gct_p {
    char *url;
    char *path;
};

int check_err(int r) {
    if (r < 0) {
        log_msg("%s\n", giterr_last()->message);
    }
    return r;
}

/**
 * \brief Initialize a repo at path
 */
int init_repo(char *path, git_repository **repo) {
    return git_repository_init(repo, path, false);
}

/**
 * \brief Clone a repo from url to path
 */
int clone_repo(char *url, char *path, git_repository **repo) {
    git_clone_options opts;
    git_clone_init_options(&opts, GIT_CLONE_OPTIONS_VERSION);
    opts.bare = 1;
    return check_err(git_clone(repo, url, path, &opts));
}

#ifdef HAVE_PTHREAD
void *clone_threaded(void *p) {
    struct gct_p *param = (struct gct_p *)p;
    log_msg("Cloning %s to %s\n", param->url, param->path);

    struct git_repository *r;
    clone_repo(param->url, param->path, &r);
    git_repository_free(r);

    free(param->path);
    free(param);
    return (void *)NULL;
}

void clone_all(struct confmgr *c) {
    struct thread_pool *pool = create_thread_pool(4);
    char *method, *domain, *path;
    method = domain = path = NULL;

    for (size_t i = 0; i < c->repo_count; i++) {
        if (match_uri(c->repo_urls[i], &method, &domain, &path) != 0) {
            log_msg("URL %s not valid, ignoring\n", c->repo_urls[i]);
            continue;
        }
        int dir_path_len =
            strlen(c->store) + 1 + strlen(domain) + strlen(path) + 1;

        char *p = xmalloc(dir_path_len * sizeof(char));
        sprintf(p, "%s/%s%s", c->store, domain, path);

        if (dir_exists(p)) {
            log_msg("Repository %s already exists, not cloning.\n", p);
            free(p);
        } else {
            struct gct_p *param = malloc(sizeof(struct gct_p));
            param->url = c->repo_urls[i];
            param->path = p;
            launch_thread(pool, clone_threaded, param);
        }
        free(method);
        free(domain);
        free(path);
    }
    join_all(pool);
    free_thread_pool(pool);
}

#else

void clone_all(struct confmgr *c) {
    char **dirs_paths = xcalloc(c->repo_count, sizeof(char *));
    char *method, *domain, *path;
    method = domain = path = NULL;
    for (size_t i = 0; i < c->repo_count; i++) {
        if (match_uri(c->repo_urls[i], &method, &domain, &path) != 0) {
            log_msg("URL %s not valid, ignoring\n", c->repo_urls[i]);
            continue;
        }
        int dir_path_len =
            strlen(c->store) + 1 + strlen(domain) + strlen(path) + 1;

        dirs_paths[i] = xrealloc(dirs_paths[i], sizeof(char) * dir_path_len);

        sprintf(dirs_paths[i], "%s/%s%s", c->store, domain, path);

        if (dir_exists(dirs_paths[i])) {
            log_msg("Repository %s already exists, not cloning.\n",
                    dirs_paths[i]);
            free(dirs_paths[i]);
        } else {
            git_repository *r = NULL;
            log_msg("Cloning %s to %s. This may take a while...\n",
                    c->repo_urls[i], dirs_paths[i]);

            clone_repo(c->repo_urls[i], dirs_paths[i], &r);
            git_repository_free(r);
            free(dirs_paths[i]);
        }
        free(domain);
        free(method);
        free(path);
    }
    free(dirs_paths);
}

#endif
