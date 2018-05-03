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
 * \file confmgr.c
 * \brief Implementation of confmgr.h
 */

#include "confmgr.h"
#include <errno.h>
#include <git2.h>
#include "gitmgr.h"
#include "err_codes.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/**
 * \brief Read a line from a file and write it to line.
 *
 * Resize line and writes the new size to len if the buffer isn't big enough.
 * Behaves a bit like GNU's getline.
 */
int getline(char **line, size_t *len, FILE *f) {
    char curr;
    size_t i = 0;
    for (;; i++) {
        int count = fread(&curr, sizeof(char), 1, f);
        if (count != 1)
            return i;
        if (curr == '\n')
            break;
        if(curr == '#'){
                while((fread(&curr, sizeof(char), 1, f)) != 0 && curr != '\n');
                if(i != 0){
                        break;
                }
                return getline(line, len, f);
        }
        (*line)[i] = curr;
        if (i == *len - 2) {
            *line = realloc(*line, *len * 2);
            memset(*line + *len, 0, *len);
            *len *= 2;
        }
    }
    return i;
}

/**
 * \brief Read the config at path
 */
struct confmgr *read_config(char *path) {
    FILE *f = fopen(path, "r");
    if (!f)
        exit(CONFIG_FILE_NOT_FOUND);
    char *store = NULL;
    size_t repo_count = 10;
    size_t repo_index = 0;
    char **repos = calloc(10, sizeof(char *));
    char *line = calloc(100, sizeof(char));
    size_t len = 100;
    int read;

    while ((read = getline(&line, &len, f)) != 0) {
        if (read < 3) {
            log_msg("Didn't expect %s\n", line);
            exit(CONFIG_FILE_MALFORMED);
        }
        if (read == 3) {
            if (strcmp(line, "end") == 0) {
                break;
            } else {
                log_msg("Didn't expect %s\n", line);
                exit(CONFIG_FILE_MALFORMED);
            }
        }
        if (!(strncmp(line, "repo=", 5) == 0)) {
            if (!(strncmp(line, "store=", 6) == 0)) {
                log_msg("Didn't expect %s\n", line);
                exit(CONFIG_FILE_MALFORMED);
            } else {
                store = calloc(read - 6 + 1, sizeof(char));
                if (read == 6) {
                    log_msg("The path needs to be set\n");
                    exit(CONFIG_FILE_MALFORMED);
                }
                strcpy(store, line + 6);
            }
        } else {
            if (read == 5) {
                log_msg("The URL needs to be set\n");
                exit(CONFIG_FILE_MALFORMED);
            }
            if (repo_index == repo_count) {
                repos = realloc(repos, repo_count * 2 * sizeof(char *));
                memset(repos + repo_count, 0, repo_count * sizeof(char *));
                repo_count *= 2;
            }
            repos[repo_index] = calloc(read - 5 + 1, sizeof(char));
            strcpy(repos[repo_index], line + 5);
            repo_index++;
        }
        memset(line, 0, len * sizeof(char));
    }

    if(store == NULL){
            log_msg("Store has to be set\n");
            exit(CONFIG_FILE_MALFORMED);
    }
    if(repo_count == 0){
            log_msg("You haven't set any repos...\n");
    }

    struct confmgr *c = malloc(sizeof(struct confmgr));
    c->store = store;
    c->repo_urls = repos;
    c->repo_count = repo_index;
    c->repo_length = repo_count;

    free(line);
    fclose(f);

    return c;
}

/**
 * \brief Free the memory used by a confmgr struct
 */
void free_confmgr(struct confmgr *c){
        free(c->store);
        for(size_t i = 0; i < c->repo_count; i++){
                free(c->repo_urls[i]);
        }
        free(c->repo_urls);
        free(c);
}

/**
 * \brief Apply the config to the filesystem
 */
void apply_conf(struct confmgr *c){
        DIR *d = opendir(c->store);
        if(!d){
                log_msg("The store %s doesn't exist yet!\n", c->store);

                int err;
                if((err = mkdir(c->store, 0777)) == -1){
                        log_msg("Couldn't create the store! Error %d\n", errno);
                        exit(STORE_CREATION_FAILED);
                }
                log_msg("Cloning all repos, this may take a while\n");
                clone_all(c);
                return;
        }
        closedir(d);
}
