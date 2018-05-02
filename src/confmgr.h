#include <stdlib.h>

#ifndef CONFMGR_H
#define CONFMGR_H

struct confmgr {
        char *store;
        char **repo_urls;
        size_t repo_count;
        size_t repo_length;
};

struct confmgr *read_config(char *path);
void free_confmgr(struct confmgr *c);
void apply_conf(struct confmgr *c);

#endif
