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
 * \file thread_pool.c
 * \brief Implementation of thread_pool.h
 */

#include "thread_pool.h"
#include "logging.h"
#include "memutils.h"
#include <stdio.h>
#include <unistd.h>

struct th_data {
    void *(*user_routine)(void *);
    void *user_arg;
    struct thread_pool *pool;
    pthread_t tid;
    size_t index;
};

struct thread_pool *create_thread_pool(size_t count) {
    struct thread_pool *pool = xmalloc(sizeof(struct thread_pool));
    pool->user_threads = xcalloc(count, sizeof(pthread_t));
    pool->th_nbr = count;
    pool->avlbl_count = pool->th_nbr;
    pthread_mutex_init(&pool->lock, NULL);
    pipe(pool->fd);
    log_msg("Writing to the pipe\n");
    for (size_t i = 0; i < count; i++)
        write(pool->fd[1], &i, sizeof(size_t));
    log_msg("Wrote to the pipe\n");
    return pool;
}

void cleanup_routine(void *th_data) {
    struct th_data *data = (struct th_data *)th_data;
    log_msg("Cleaning up after thread %lu!\n", data->index);
    pthread_mutex_lock(&data->pool->lock);
    data->pool->avlbl_count++;
    write(data->pool->fd[1], &(data->index), sizeof(size_t));
    pthread_mutex_unlock(&data->pool->lock);
}

void *start_user_routine(void *th_data) {
    void *ret;
    pthread_cleanup_push(cleanup_routine, th_data);

    struct th_data *data = (struct th_data *)th_data;

    log_msg("Starting thread %lu\n", data->index);

    ret = data->user_routine(data->user_arg);

    pthread_cleanup_pop(1);

    return ret;
}

void launch_thread(struct thread_pool *pool, void *(*routine)(void *),
                   void *arg) {
    size_t next_index;
    log_msg("Reading from the pipe\n");
    read(pool->fd[0], &next_index, sizeof(size_t));
    log_msg("Available thread at index %lu\n", next_index);
    struct th_data *param = malloc(sizeof(struct th_data));
    param->user_routine = routine;
    param->user_arg = arg;
    param->pool = pool;
    param->index = next_index;
    param->tid = pool->user_threads[next_index];
    pthread_mutex_lock(&pool->lock);
    pool->avlbl_count--;
    log_msg("Creating a new thread with tid %lu\n",
            pool->user_threads[next_index]);
    pthread_create(pool->user_threads + next_index, NULL, start_user_routine,
                   param);
    pthread_mutex_unlock(&pool->lock);
}

void join_all(struct thread_pool *pool) {
    log_msg("Joining all threads\n");
    while (pool->avlbl_count != pool->th_nbr) {
        log_msg("%d / %d threads have stopped\n", pool->avlbl_count,
                pool->th_nbr);
        size_t next_index;
        read(pool->fd[0], &next_index, sizeof(size_t));
        pthread_join(pool->user_threads[next_index], NULL);
    }
    log_msg("All threads have joined\n");
    for(size_t i = 0; i < pool->th_nbr; i++)
            write(pool->fd[1], &i, sizeof(size_t));
}
