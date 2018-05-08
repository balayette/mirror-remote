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

#include "logging.h"
#include "memutils.h"
#include "thread_pool.h"
#include <stdio.h>
#include <unistd.h>

struct th_data {
    void *(*user_routine)(void *);
    void *user_arg;
    struct thread_pool *pool;
    pthread_t tid;
    size_t index;
};

/**
 * \brief Create a thread pool with count slots
 */
struct thread_pool *create_thread_pool(size_t count) {
    struct thread_pool *pool = xmalloc(sizeof(struct thread_pool));
    pool->user_threads = xcalloc(count, sizeof(pthread_t));
    pool->th_nbr = count;
    pool->avlbl_count = pool->th_nbr;
    pthread_mutex_init(&pool->lock, NULL);
    pipe(pool->finished_fd);
    pipe(pool->avlbl_fd);
    for (pool->avlbl_count = 0; pool->avlbl_count < count; pool->avlbl_count++)
        write(pool->avlbl_fd[1], &pool->avlbl_count, sizeof(size_t));
    return pool;
}

void cleanup_routine(void *th_data) {
    struct th_data *data = (struct th_data *)th_data;
    pthread_mutex_lock(&data->pool->lock);
    data->pool->avlbl_count++;
    write(data->pool->finished_fd[1], &(data->index), sizeof(size_t));
    pthread_mutex_unlock(&data->pool->lock);
}

void *start_user_routine(void *th_data) {
    void *ret;
    pthread_cleanup_push(cleanup_routine, th_data);

    struct th_data *data = (struct th_data *)th_data;

    ret = data->user_routine(data->user_arg);

    pthread_cleanup_pop(1);

    free(th_data);

    return ret;
}

/**
 * \brief Starts routine(arg) in a thread, blocks if there are no slots
 * available
 */
void launch_thread(struct thread_pool *pool, void *(*routine)(void *), void *arg) {
    size_t next_index;
    read(pool->avlbl_fd[0], &next_index, sizeof(size_t));
    struct th_data *param = malloc(sizeof(struct th_data));
    param->user_routine = routine;
    param->user_arg = arg;
    param->pool = pool;
    param->index = next_index;
    param->tid = pool->user_threads[next_index];

    pthread_mutex_lock(&pool->lock);
    pool->avlbl_count--;
    pthread_create(pool->user_threads + next_index, NULL, start_user_routine,
                   param);
    pthread_mutex_unlock(&pool->lock);
}

/**
 * \brief Waits for all threads in the pool to join
 */
void join_all(struct thread_pool *pool) {
    while (pool->avlbl_count != pool->th_nbr) {
        size_t next_index;
        read(pool->finished_fd[0], &next_index, sizeof(size_t));
        pthread_join(pool->user_threads[next_index], NULL);
        write(pool->avlbl_fd[1], &next_index, sizeof(size_t));
    }
}

/**
 * \brief Free the memory allocated by the pool
 */
void free_thread_pool(struct thread_pool *pool) {
    join_all(pool);

    close(pool->avlbl_fd[0]);
    close(pool->avlbl_fd[1]);
    close(pool->finished_fd[0]);
    close(pool->finished_fd[1]);

    free(pool->user_threads);

    free(pool);
}
