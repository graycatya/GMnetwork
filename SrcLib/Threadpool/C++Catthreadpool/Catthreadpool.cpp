#include "Catthreadpool.h"
#include <unistd.h>

using namespace CATTHREADPOOL;

void* CatThreadPool::adjust_thread(void *threadpool)
{
    int i;
    CatThreadPool *pool = (CatThreadPool *)threadpool;
    while(!pool->shutdown)
    {
        sleep(pool->tick_time);  

        pthread_mutex_lock(&(pool->my_lock));
        int queue_size = pool->threads.size();
        //int queue_size = pool->queue_size;
        int live_thread_num = pool->live_thread_num;
        pthread_mutex_unlock(&(pool->my_lock));
        
        std::unique_lock<std::mutex> locke(pool->thread_counter_lock);
        int busy_thread_num = pool->busy_thread_num;
        locke.unlock();

        if(queue_size >= pool->min_thread_num && live_thread_num < pool->max_thread_num)
        {
            std::unique_lock<std::mutex> locke(pool->my_lock);
            int add = 0;

            for(i = 0; i < pool->max_thread_num && add < pool->add_threadnum 
                && pool->live_thread_num < pool->max_thread_num; i++)
            {
                std::thread* threadpool_thread = new std::thread(&threadpool_thread, (void*)pool);
                add++;
                pool->live_thread_num++;
                pool->thread_id.push_back(threadpool_thread);
            }

            locke.unlock();
        }

        /* 销毁多余的空间线程 算法：忙线程*2 小于 存活的线程数 且 存活的线程数 大于 最小的线程数时 */
        if( (busy_thread_num * 2) < live_thread_num && live_thread_num > pool->min_thread_num )
        {
            for(i = 0; i < pool->wait_exit_thread_num; i++)
            {
                pool->queue_not_empty.notify_one();
            }
        }
    }
    return nullptr;
}

void* CatThreadPool::threadpool_thread(void *threadpool)
{
    CatThreadPool *pool = (CatThreadPool *)threadpool;
    while(true)
    {
        std::unique_lock<std::mutex> locke(pool->my_lock);
        
        while((pool->thread_id.size() == 0) && (!pool->shutdown))
        {
            pool->queue_not_empty.wait(locke);

            if(pool->wait_exit_thread_num > 0)
            {
                pool->wait_exit_thread_num--;

                if(pool->live_thread_num > pool->min_thread_num)
                {
                    pool->live_thread_num--;
                    locke.unlock();
                    //std::thread::get_id();
                    return nullptr;
                }
            }
        }
        
        if(pool->shutdown)
        {
            locke.unlock();
            return nullptr;
        }
        threadevent_t *event = (threadevent_t*)pool->task_queue.front();

        pool->queue_not_full.notify_all();
        locke.unlock();
        //pool->task_queue.
        //event.function = pool->task_queue

        std::unique_lock<std::mutex> locke(pool->thread_counter_lock);
        pool->busy_thread_num++;
        locke.unlock();

        (*(event->function))(event->arg);

        std::unique_lock<std::mutex> locke(pool->thread_counter_lock);
        pool->busy_thread_num--;
        locke.unlock();

    }

    return nullptr;
}

CatThreadPool::CatThreadPool(int max_thread_num, int min_thread_num, int queue_max_size)
{
    this->max_thread_num = max_thread_num;
    this->min_thread_num = min_thread_num;
    this->busy_thread_num = 0;
    this->live_thread_num = min_thread_num; /* init survive */
    this->wait_exit_thread_num = 0;
    this->queue_max_size = queue_max_size;
    this->shutdown = false; /* no thread */

    threads = new (pthread_t *)[max_thread_num];
    if(threads == nullptr)
    {
        printf("new threads fail\n");
        return;
    }

    if(pthread_mutex_init(&(my_lock), nullptr) != 0
    || pthread_mutex_init(&(thread_counter_lock), nullptr) != 0
    || pthread_cond_init(&(queue_not_empty), nullptr) != 0
    || pthread_cond_init(&(queue_not_full),nullptr) != 0)
    {
        printf("init the lock or cond fail\n");
        return;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CANCEL_DEFERRED);
    for(int i = 0; i < min_thread_num; i++)
    {
        pthread_create(&(threads[i]), &attr, threadpool_thread, (void*)this);
    }

    pthread_create(&adjust_tid, &attr, adjust_thread, (void*)this);

}

CatThreadPool::~CatThreadPool()
{

}

int CatThreadPool::AddThreads(threadevent_t* threadevent)
{
    int size = -1;
    if(threadevent == nullptr)
    {
        return -1;
    }
    pthread_mutex_lock(&(my_lock));
    while( ((size = this->task_queue.size()) == queue_max_size) && (!shutdown) )
    {
        pthread_cond_wait(&(queue_not_full), &(my_lock));
    }

    if(shutdown)
    {
        pthread_cond_broadcast(&(queue_not_empty));
        pthread_mutex_unlock(&(my_lock));
        return 0;
    }

    task_queue.push_back(threadevent);

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