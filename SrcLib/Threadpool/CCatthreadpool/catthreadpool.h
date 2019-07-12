#ifndef _CATTHREADPOOL_H_
#define _CATTHREADPOOL_H_

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#ifdef  __cplusplus  
extern "C" {  
#endif  

enum {
    false,
    true
};

typedef struct 
{
    /* data */
    void *(*function)(void *);
    void *arg;
} threadpool_task_t;

struct threadpool_t
{
    pthread_mutex_t my_lock;                 /* 用于锁住本结构体 */
    pthread_mutex_t thread_counter_lock;     /* 记录忙状态线程个数的锁 */

    pthread_cond_t queue_not_full;    /* 当任务队列满时， 添加任务的线程阻塞， */
    pthread_cond_t queue_not_empty;   /* 任务队列里不为空时，ongzhi通知等待任务线程 */

    pthread_t *threads;     /* 存放线程池中每个线程的tid。数组 */
    pthread_t adjust_tid;   /* 管理线程tid */
    threadpool_task_t *task_queue;  /* 任务队列(数组首地址) */
    void *task_queue_custom;    /* 自定义任务队列 */

    int max_thread_num;         /* 线程池最小线程数 */
    int min_thread_num;         /* 线程池最大线程数 */
    int live_thread_num;        /* 当前存活线程个数 */
    int busy_thread_num;        /* 忙状态线程个数 */
    int wait_exit_thread_num;   /* 要销毁的线程个数 */

    int queue_front;        /* task_queue队头下标 */
    int queue_rear;         /* task_queue队尾下标 */
    int queue_size;         /* task_queue队中实际任务数 */
    int queue_max_size;     /* task_queue队列可容纳任务上限 */

    int shutdown;       /* 标志位，线程池使用状态，true or false */
};

typedef struct threadpool_t threadpool_t;

threadpool_t *threadpool_create(int min_thread_num, int max_thread_num, int queue_max_size);
int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg);
int threadpool_destroy(threadpool_t *pool);
int threadpool_all_threadnum(threadpool_t *pool);
int threadpool_busy_threadnum(threadpool_t *pool);

#ifdef  __cplusplus  
}  
#endif  /* end of __cplusplus */

#endif