#include "Threadpool.h"
#include <unistd.h>

using namespace CATTHREADPOOL;

int is_thread_alive(pthread_t tid);

CatThreadPool::CatThreadPool(int max_thread_num, int min_thread_num, int queue_max_size)
    : max_thread_num(max_thread_num), min_thread_num(min_thread_num), queue_max_size(queue_max_size)
{
    threads = new pthread_t[max_thread_num];
    if(pthread_mutex_init(&(my_lock), NULL) != 0
        || pthread_mutex_init(&(thread_counter_lock), NULL) != 0
        || pthread_cond_init(&(queue_not_empty), NULL) != 0
        || pthread_cond_init(&(queue_not_full), NULL) != 0)
        {
            printf("init the lock or cond fail\n");
            return;
        }

    //启动工作线程
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //线程分离 - 不关心结束状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CANCEL_DEFERRED);
    for(int i = 0; i < min_thread_num; i++)
    {
        pthread_create(&(threads[i]), &attr, threadpool_thread, (void *)this);
        //printf("start thread 0x%x...\n", (unsigned int)pool->threads[i]);
    }

    //管理者线程
    pthread_create(&(adjust_tid), &attr, adjust_thread, (void *)this);
}
CatThreadPool::~CatThreadPool()
{

    if(threads != nullptr)
    {
        shutdown = true;

        for(int i = 0; i < live_thread_num; i++)
        {
            pthread_cond_broadcast(&(queue_not_empty));
        }

        delete[] threads;
        pthread_mutex_lock(&(my_lock));
        pthread_mutex_destroy(&(my_lock));
        pthread_mutex_lock(&(thread_counter_lock));
        pthread_mutex_destroy(&(thread_counter_lock));
        pthread_cond_destroy(&(queue_not_empty));
        pthread_cond_destroy(&(queue_not_full));
    }
}

int CatThreadPool::AddThreads(void*(*func)(void* arg), void* arg)
{
    pthread_mutex_lock(&(my_lock));
    /* 为真时，队列已经满，调用wait阻塞 */
    while((task_queue.size() == queue_max_size) && (!shutdown))
    {
        pthread_cond_wait(&(queue_not_full), &(my_lock));
    }

    if(shutdown)
    {
        //唤醒所有条件变量
        pthread_cond_broadcast(&(queue_not_empty));
        pthread_mutex_unlock(&(my_lock));
        return 0;
    }

    /* 添加任务到队列里 */
    threadevent_t* event = new threadevent_t(func, arg);
    task_queue.push_back(event);

    /* 添加完任务后，队列不为空，唤醒线程池中 等待处理的任务 */
    pthread_cond_signal(&(queue_not_empty));
    pthread_mutex_unlock(&(my_lock));
    
    return 0;
}

int CatThreadPool::GetExistThreadNum( void )
{
    int num = -1;
    pthread_mutex_lock(&(my_lock));
    num = live_thread_num;
    pthread_mutex_unlock(&(my_lock));
    return num;
}

int CatThreadPool::GetBusyThreadNum( void )
{
    int num = -1;
    pthread_mutex_lock(&(thread_counter_lock));
    num = busy_thread_num;
    pthread_mutex_unlock(&(thread_counter_lock));
    return num;
}

void* CatThreadPool::adjust_thread(void *threadpool)
{
int i;
    CatThreadPool *pool = (CatThreadPool *)threadpool;
    while(!pool->shutdown)
    {
        sleep(pool->tick_time);    /* 定时 线程池管理周期 */

        pthread_mutex_lock(&(pool->my_lock));
        int queue_size = pool->task_queue.size();
        int live_thread_num = pool->live_thread_num;
        pthread_mutex_unlock(&(pool->my_lock));

        pthread_mutex_lock(&(pool->thread_counter_lock));
        int busy_thread_num = pool->busy_thread_num;
        pthread_mutex_unlock(&(pool->thread_counter_lock));

        /* 创建新线程 算法: 任务数大于最小线程池个数，且存活的线程数少于最大线程个数时 */
        if(queue_size >= pool->min_thread_num && live_thread_num < pool->max_thread_num)
        {
            pthread_mutex_lock(&(pool->my_lock));
            int add = 0;

            /* 一次增加 DEFAULT_THREAD_VARY 个线程 */
            for(i = 0; i < pool->max_thread_num && add < pool->add_threadnum 
                && pool->live_thread_num < pool->max_thread_num; i++)
            {
                if(pool->threads[i] == 0 || !is_thread_alive(pool->threads[i]))
                {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thread_num++;
                }
            }
            
            pthread_mutex_unlock(&(pool->my_lock));

        }
        /* 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时 */
        if((busy_thread_num * 2) < live_thread_num && live_thread_num > pool->min_thread_num)
        {
            pthread_mutex_lock(&(pool->my_lock));
            pool->wait_exit_thread_num = pool->del_threadnum;
            pthread_mutex_unlock(&(pool->my_lock));

            for(i = 0; i < pool->del_threadnum; i++)
            {
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }      

    }

    return nullptr;
}

void* CatThreadPool::threadpool_thread(void *threadpool)
{
    CatThreadPool *pool = (CatThreadPool *)threadpool;
    //threadpool_task_t task;

    while(true)
    {
        /*刚创建出线程，等待任务队列里有任务，否则阻塞等待任务队列里有任务后再唤醒接收任务*/
        pthread_mutex_lock(&(pool->my_lock));

        /*queue_size == 0 说明没有任务，调 wait 阻塞在条件变量上, 若有任务，跳过该while*/
        while((pool->task_queue.size() == 0) && (!pool->shutdown))
        {
            //printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->my_lock));

            /* 清除指定数目的空闲线程，如果要结束的线程个数大于0，结束线程 */
            if(pool->wait_exit_thread_num > 0)
            {
                pool->wait_exit_thread_num--;

                /* 如果线程池里线程个数大于最小值时可以结束当前线程 */
                if(pool->live_thread_num > pool->min_thread_num)
                {
                    //printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                    pool->live_thread_num--;
                    pthread_mutex_unlock(&(pool->my_lock));
                    pthread_exit(NULL);
                }
            }
        }

        /* 销毁线程 */
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->my_lock));
            //printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
            pthread_exit(NULL);
        }

        /* 出队操作 - 从任务队列里获取任务 */
        threadevent_t* event = (threadevent_t*)pool->task_queue.front();
        pool->task_queue.pop_front();

        /* 通知可以有新的任务添加进来 */
        pthread_cond_broadcast(&(pool->queue_not_full));

        /* 任务取出后，立即将线程池锁 释放 */
        pthread_mutex_unlock(&(pool->my_lock));

        /* 执行任务 */
        //printf("thread 0x%x start working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter_lock));
        pool->busy_thread_num++;    /* 防止出现脏写 */
        pthread_mutex_unlock(&(pool->thread_counter_lock));

        (*(event->function))(event->arg);   /* 执行回调 */

        //printf("thread 0x%x end working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter_lock));
        delete event;
        pool->busy_thread_num--;
        pthread_mutex_unlock(&(pool->thread_counter_lock));

    }

    pthread_exit(NULL);
}

int is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);
    if(kill_rc == ESRCH)
    {
        return false;
    }
    return true;
}