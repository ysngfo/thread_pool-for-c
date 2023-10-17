#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include"thread_pool.h"
#define THREAD_POOL_SIZE 4

// ��ʼ���̳߳�
void initialize_pool(ThreadPool *pool, int size) {
    pool->task_queue = (Task *)malloc(sizeof(Task) * size);
    pool->queue_size = size;
    pool->front = pool->rear = pool->count = 0;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);
}

// ������������
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

// �Ӷ�����ȡ������
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

// �̹߳�������������ִ����������е�����
void *worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (1) {
        Task task = dequeue_task(pool);
        task.function(task.data);
    }
    return NULL;
}

// �����̳߳ز������߳�
void create_thread_pool(ThreadPool *pool, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, worker_thread, pool);
        pthread_detach(thread);
    }
}

// �ύ�����̳߳�
void submit_task(ThreadPool *pool, void (*function)(void *), void *data) {
    Task task = {function, data};
    enqueue_task(pool, task);
}

// �����̳߳���Դ
void cleanup_pool(ThreadPool *pool) {
    free(pool->task_queue);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);
}

// ʾ��������
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
