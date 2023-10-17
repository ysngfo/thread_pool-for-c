#ifndef C2828BFE_A40D_4F42_8B17_62DE88EBD3CE
#define C2828BFE_A40D_4F42_8B17_62DE88EBD3CE
typedef struct {
    void (*function)(void *); // 任务的函数指针
    void *data; // 任务的参数
} Task;

// 线程池结构体，包含任务队列和相关同步元素
typedef struct {
    Task *task_queue; // 任务队列
    int queue_size; // 队列大小
    int front, rear; // 队列前后索引
    int count; // 队列中的任务数
    pthread_mutex_t lock; // 互斥锁，用于同步访问任务队列
    pthread_cond_t not_empty; // 条件变量，用于通知队列非空
    pthread_cond_t not_full; // 条件变量，用于通知队列非满
} ThreadPool;

void initialize_pool(ThreadPool *pool, int size);
void enqueue_task(ThreadPool *pool, Task task);
Task dequeue_task(ThreadPool *pool);
void *worker_thread(void *arg);
void create_thread_pool(ThreadPool *pool, int num_threads);
void submit_task(ThreadPool *pool, void (*function)(void *), void *data);
void cleanup_pool(ThreadPool *pool);

#endif /* C2828BFE_A40D_4F42_8B17_62DE88EBD3CE */
