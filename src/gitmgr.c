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
#include "../config.h"
#include "confmgr.h"
#include "logging.h"
#include "memutils.h"
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
    int th_id;
    int *pipe;
};

void *git_clone_worker(void *p) {
    struct gct_p *params = (struct gct_p *)p;
    struct git_repository *r = NULL;
    clone_repo(params->url, params->path, &r);
    git_repository_free(r);
    write(params->pipe[1], &(params->th_id), sizeof(int));
    log_msg("Thread %d : Finished cloning %s\n", params->th_id, params->url);
    return (void *)0;
}

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

/**
 * \brief The core of clone_all, but uses pthreads
 */
void clone_all_pthread(struct confmgr *c, int i, pthread_t *threads,
                       struct gct_p **thread_params, int thread_count,
                       char **dirs_paths, int pipefd[2]) {
    log_msg("Starting a new thread to clone %s\n", c->repo_urls[i]);
    struct gct_p *params = xmalloc(sizeof(struct gct_p));
    thread_params[i] = params;
    params->url = c->repo_urls[i];
    params->path = dirs_paths[i];
    params->th_id = thread_count;
    params->pipe = pipefd;
    pthread_create(threads + i, NULL, git_clone_worker, (void *)params);
}

/**
 * \brief Clone all repos stored in a configmgr
 */
void clone_all(struct confmgr *c) {
#ifdef HAVE_PTHREAD

    pthread_t *threads = xcalloc(c->repo_count, sizeof(pthread_t));
    struct gct_p **thread_params = xcalloc(c->repo_count, sizeof(pthread_t));
    int th_id = 0;
    int pipefd[2];
    pipe(pipefd);

#endif
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

        dirs_paths[i] = xrealloc(dirs_paths[i], sizeof(char) * (dir_path_len));

        sprintf(dirs_paths[i], "%s/%s%s", c->store, domain, path);

        if (dir_exists(dirs_paths[i])) {
            log_msg("Repository %s already exists, not cloning.\n",
                    dirs_paths[i]);
            free(dirs_paths[i]);
        } else {
#ifdef HAVE_PTHREAD

            clone_all_pthread(c, i, threads, thread_params, th_id++, dirs_paths,
                              pipefd);

#else

            git_repository *r = NULL;
            log_msg("Cloning %s to %s. This may take a while...\n",
                    c->repo_urls[i], dirs_paths[i]);

            clone_repo(c->repo_urls[i], dirs_paths[i], &r);
            git_repository_free(r);
            free(dirs_paths[i]);

#endif
        }
        free(domain);
        free(method);
        free(path);
    }

#ifdef HAVE_PTHREAD

    for (int i = 0; i < th_id; i++) {
        int idx;
        read(pipefd[0], &idx, sizeof(int));
        pthread_join(threads[idx], NULL);
        free(thread_params[idx]);
    }
    free(threads);
    free(thread_params);
    close(pipefd[0]);
    close(pipefd[1]);
#endif
    free(dirs_paths);
}
