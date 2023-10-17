#ifndef C2828BFE_A40D_4F42_8B17_62DE88EBD3CE
#define C2828BFE_A40D_4F42_8B17_62DE88EBD3CE
typedef struct {
    void (*function)(void *); // ����ĺ���ָ��
    void *data; // ����Ĳ���
} Task;

// �̳߳ؽṹ�壬����������к����ͬ��Ԫ��
typedef struct {
    Task *task_queue; // �������
    int queue_size; // ���д�С
    int front, rear; // ����ǰ������
    int count; // �����е�������
    pthread_mutex_t lock; // ������������ͬ�������������
    pthread_cond_t not_empty; // ��������������֪ͨ���зǿ�
    pthread_cond_t not_full; // ��������������֪ͨ���з���
} ThreadPool;

void initialize_pool(ThreadPool *pool, int size);
void enqueue_task(ThreadPool *pool, Task task);
Task dequeue_task(ThreadPool *pool);
void *worker_thread(void *arg);
void create_thread_pool(ThreadPool *pool, int num_threads);
void submit_task(ThreadPool *pool, void (*function)(void *), void *data);
void cleanup_pool(ThreadPool *pool);

#endif /* C2828BFE_A40D_4F42_8B17_62DE88EBD3CE */
