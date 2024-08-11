#ifndef _THREAD_POOL_H_INCLUDED_
#define _THREAD_POOL_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"

#define DEFAULT_THREADS_NUM 4
#define DEFAULT_QUEUE_NUM  65535


typedef unsigned long         atomic_uint_t;
typedef struct thread_task_s  thread_task_t;
typedef struct thread_pool_s  thread_pool_t;

/* 任务结构 */
struct thread_task_s {
    thread_task_t       *next;  // 指向下一个节点
    uint_t               id;    // 任务 id
    void                *ctx;   // 回调函数参数
    void               (*handler)(void *data);  // 回调函数
};

/* 任务队列 */
typedef struct {
    thread_task_t        *first;
    thread_task_t        **last;    // 二级指针，指向最后一个节点的下一个节点，添加节点方便
} thread_pool_queue_t;

#define thread_pool_queue_init(q)                                         \
    (q)->first = NULL;                                                    \
    (q)->last = &(q)->first


struct thread_pool_s {
    pthread_mutex_t        mtx; // 锁住任务队列，
    thread_pool_queue_t   queue;    // 任务队列
    int_t                 waiting;  /*线程池中还有没有处理的任务还有多少*/
    pthread_cond_t         cond;    // 任务队列 是否为空

    char                  *name;    /*线程池名字*/
    uint_t                threads;  /*线程数量*/
    int_t                 max_queue;    /*任务队列最大长度*/
};

thread_task_t *thread_task_alloc(size_t size);
void thread_task_free(thread_task_t* task);
int_t thread_task_post(thread_pool_t *tp, thread_task_t *task);
thread_pool_t* thread_pool_init();
void thread_pool_destroy(thread_pool_t *tp);

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_POOL_H_INCLUDED_ */
