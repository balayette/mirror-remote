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
#include "gitmgr.h"
#include "confmgr.h"
#include "logging.h"
#include "utils.h"
#include <git2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int check_err(int r) {
    if (r < 0) {
        log_msg("%s\n", giterr_last()->message);
    }
    return r;
}

int init_repo(char *path, git_repository **repo) {
    return git_repository_init(repo, path, false);
}

int clone_repo(char *url, char *path, git_repository **repo) {
    git_clone_options opts;
    git_clone_init_options(&opts, GIT_CLONE_OPTIONS_VERSION);
    opts.bare = 1;
    return check_err(git_clone(repo, url, path, &opts));
}

/**
 * \brief Clone all repos stored in a configmgr
 */
void clone_all(struct confmgr *c) {
    git_repository *r = NULL;
    char *dir_path = NULL;
    (void)r;
    (void)dir_path;
    char *method, *domain, *path;
    method = domain = path = NULL;
    for (size_t i = 0; i < c->repo_count; i++) {
        if (match_uri(c->repo_urls[i], &method, &domain, &path) != 0) {
            log_msg("URL %s not valid, ignoring\n", c->repo_urls[i]);
            continue;
        }
        int dir_path_len =
            strlen(c->store) + 1 + strlen(domain) + strlen(path) + 1;
        dir_path = realloc(dir_path, sizeof(char) * (dir_path_len));
        memset(dir_path, 0, dir_path_len * sizeof(char));

        sprintf(dir_path, "%s/%s%s", c->store, domain, path);

        if (dir_exists(dir_path)) {
            log_msg("Repository %s already exists, not cloning.\n", dir_path);
            continue;
        }
        log_msg("Cloning %s to %s. This may take a while...\n", c->repo_urls[i],
                dir_path);

        clone_repo(c->repo_urls[i], dir_path, &r);

        git_repository_free(r);

        free(method);
        free(domain);
        free(path);
    }
    free(dir_path);
}
