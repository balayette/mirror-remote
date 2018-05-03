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
 * \file thread_pool.h
 * \brief pthread pool
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../config.h"

#include <pthread.h>
#include <stdbool.h>

// TODO : make it work with th_nbr > than the number of threads the user
// wants to start.
// Will have to use 2 FDs, I think
struct thread_pool {
        size_t th_nbr;
        pthread_t *user_threads;
        int fd[2];
        size_t avlbl_count;
        pthread_mutex_t lock;
};

struct thread_pool *create_thread_pool(size_t count);

void launch_thread(struct thread_pool *pool, void *(*routine)(void *),
                   void *arg);
void join_all(struct thread_pool *pool);

#endif
