#include <git2.h>
#include "confmgr.h"

#ifndef GITMGR_H
#define GITMGR_H

int init_repo(char *path, git_repository **repo);
int clone_repo(char *url, char *path, git_repository **repo);
void clone_all(struct confmgr *c);

#endif
