#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include"thread_pool.h"

// 初始化线程池
void initialize_pool(ThreadPool *pool, int size) {
    pool->task_queue = (Task *)malloc(sizeof(Task) * size);
    pool->queue_size = size;
    pool->front = pool->rear = pool->count = 0;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);
}

// 将任务加入队列
void enqueue_task(ThreadPool *pool, Task task) {
    pthread_mutex_lock(&pool->lock);
    while (pool->count == pool->queue_size) {
        pthread_cond_wait(&pool->not_full, &pool->lock);
    }

    pool->task_queue[pool->rear] = task;
    pool->rear = (pool->rear + 1) % pool->queue_size;
    pool->count++;

    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);
}

// 从队列中取出任务
Task dequeue_task(ThreadPool *pool) {
    Task task;
    pthread_mutex_lock(&pool->lock);
    while (pool->count == 0) {
        pthread_cond_wait(&pool->not_empty, &pool->lock);
    }

    task = pool->task_queue[pool->front];
    pool->front = (pool->front + 1) % pool->queue_size;
    pool->count--;

    pthread_cond_signal(&pool->not_full);
    pthread_mutex_unlock(&pool->lock);

    return task;
}

// 线程工作函数，用于执行任务队列中的任务
void *worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (1) {
        Task task = dequeue_task(pool);
        task.function(task.data);
    }
    return NULL;
}

// 创建线程池并启动线程
void create_thread_pool(ThreadPool *pool, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, worker_thread, pool);
        pthread_detach(thread);
    }
}

// 提交任务到线程池
void submit_task(ThreadPool *pool, void (*function)(void *), void *data) {
    Task task = {function, data};
    enqueue_task(pool, task);
}

// 清理线程池资源
void cleanup_pool(ThreadPool *pool) {
    free(pool->task_queue);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);
}

// 示例任务函数
void my_task_function(void *data) {
    int *number = (int *)data;
    printf("Processing number: %d\n", *number);
}

int main() {
    ThreadPool pool;
    initialize_pool(&pool, 10);
    create_thread_pool(&pool, THREAD_POOL_SIZE);

    for (int i = 0; i < 20; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        submit_task(&pool, my_task_function, number);
    }

    sleep(5);
    cleanup_pool(&pool);
    return 0;
}
