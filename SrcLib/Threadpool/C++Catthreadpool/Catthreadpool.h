#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace CATTHREADPOOL
{

    class CatThreadPool
    {
        public:
            CatThreadPool(int max_thread_num, int min_thread_num);
            ~CatThreadPool();
            int AddThread(void*(*function)(void* arg), void* arg);

            int GetExistThreadNum( void );
            int GetBusyThreadNum( void );
        
        private:
            static void* adjust_thread(CatThreadPool* arg);
            static void* threadpool_thread(void *threadpool);

        private:
            std::mutex my_lock;                 /* 用于锁住本结构体 */
            std::mutex thread_counter_lock;     /* 记录忙状态线程个数的锁 */

            std::condition_variable queue_not_full;    /* 当任务队列满时， 添加任务的线程阻塞， */
            std::condition_variable queue_not_empty;   /* 任务队列里不为空时，ongzhi通知等待任务线程 */

            std::vector<std::thread::id> thread_id;     /* 存放线程池中每个线程的tid。数组 */
            std::thread::id adjust_tid;   /* 管理线程tid */
            std::vector<void*, void*> task_queue;  /* 任务队列(数组首地址) */
            void *task_queue_custom;    /* 自定义任务队列 */

            int max_thread_num;         /* 线程池最大线程数 */
            int min_thread_num;         /* 线程池最小线程数 */
            int live_thread_num;        /* 当前存活线程个数 */
            int busy_thread_num;        /* 忙状态线程个数 */
            int wait_exit_thread_num;   /* 要销毁的线程个数 */

            int queue_front;        /* task_queue队头下标 */
            int queue_rear;         /* task_queue队尾下标 */
            int queue_size;         /* task_queue队中实际任务数 */
            int queue_max_size;     /* task_queue队列可容纳任务上限 */

            int shutdown;       /* 标志位，线程池使用状态，true or false */
    };
}