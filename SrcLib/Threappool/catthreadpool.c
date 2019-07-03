#include "catthreadpool.h"

#define DEFAULT_TIME 10                 /*10s检测一次*/
#define MIN_WAIT_TASK_NUM 10            /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/ 
#define DEFAULT_THREAD_VARY 10          /*每次创建和销毁线程的个数*/

void *threadpool_thread(void *threadpool);

void *adjust_thread(void *threadpool);

int is_thread_alive(pthread_t tid);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int min_thread_num, int max_thread_num, int queue_max_size)
{
    int i;
    threadpool_t *pool = NULL;
    do
    {
        /* code */
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL)
        {
            printf("malloc threadpool fail");
            break;
        }
        pool->min_thread_num = min_thread_num;
        pool->max_thread_num = max_thread_num;
        pool->busy_thread_num = 0;
        pool->live_thread_num = min_thread_num; /* 活着的线程数 初值=最小线程数 */
        pool->wait_exit_thread_num = 0;
        pool->queue_size = 0;                   /* 0 */
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = false;     /* 不关闭线程池 */

        /* 根据最大线程上限数，给工作线程数组开辟空间， 并清零 */
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*max_thread_num);
        if(pool->threads == NULL)
        {
            printf("malloc threads fail");
            break;
        }

        memset(pool->threads, 0, sizeof(pthread_t)*max_thread_num);
        
        /* 队列开辟空间 */
        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t)*queue_max_size);
        if(pool->task_queue == NULL)
        {
            printf("malloc task_queue fail\n");
            break;
        }

        if(pthread_mutex_init(&(pool->my_lock), NULL) != 0
            || pthread_mutex_init(&(pool->thread_counter_lock), NULL) != 0
            || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0
            || pthread_cond_init(&(pool->queue_not_full), NULL) != 0)
            {
                printf("init the lock or cond fail\n");
                break;
            }

        //启动工作线程
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        //线程分离 - 不关心结束状态
        pthread_attr_setdetachstate(&attr, PTHREAD_CANCEL_DEFERRED);
        for(i = 0; i < min_thread_num; i++)
        {
            pthread_create(&(pool->threads[i]), &attr, threadpool_thread, (void *)pool);
            printf("start thread 0x%x...\n", (unsigned int)pool->threads[i]);
        }

        //管理者线程
        pthread_create(&(pool->adjust_tid), &attr, adjust_thread, (void *)pool);

        return pool;

    } while (0);
    
    threadpool_free(pool);

    return NULL;
}

int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg)
{
    pthread_mutex_lock(&(pool->my_lock));
    /* 为真时，队列已经满，调用wait阻塞 */
    while((pool->queue_size == pool->queue_max_size) && (!pool->shutdown))
    {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->my_lock));
    }

    if(pool->shutdown)
    {
        //唤醒所有条件变量
        pthread_cond_broadcast(&(pool->queue_not_empty));
        pthread_mutex_unlock(&(pool->my_lock));
        return 0;
    }

    /* 回调函数参数 清空 */
    if(pool->task_queue[pool->queue_rear].arg != NULL)
    {
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /* 添加任务到队列里 */
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    /* 队尾指针移动，模拟环形 */
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    pool->queue_size++;

    /* 添加完任务后，队列不为空，唤醒线程池中 等待处理的任务 */
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->my_lock));
    
    return 0;
}

void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while(true)
    {
        /*刚创建出线程，等待任务队列里有任务，否则阻塞等待任务队列里有任务后再唤醒接收任务*/
        pthread_mutex_lock(&(pool->my_lock));

        /*queue_size == 0 说明没有任务，调 wait 阻塞在条件变量上, 若有任务，跳过该while*/
        while((pool->queue_size == 0) && (!pool->shutdown))
        {
            printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->my_lock));

            /* 清除指定数目的空闲线程，如果要结束的线程个数大于0，结束线程 */
            if(pool->wait_exit_thread_num > 0)
            {
                pool->wait_exit_thread_num--;

                /* 如果线程池里线程个数大于最小值时可以结束当前线程 */
                if(pool->live_thread_num > pool->min_thread_num)
                {
                    printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
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
            printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
            pthread_exit(NULL);
        }

        /* 出队操作 - 从任务队列里获取任务 */
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--;

        /* 通知可以有新的任务添加进来 */
        pthread_cond_broadcast(&(pool->queue_not_full));

        /* 任务取出后，立即将线程池锁 释放 */
        pthread_mutex_unlock(&(pool->my_lock));

        /* 执行任务 */
        printf("thread 0x%x start working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter_lock));
        pool->busy_thread_num++;    /* 防止出现脏写 */
        pthread_mutex_unlock(&(pool->thread_counter_lock));

        (*(task.function))(task.arg);   /* 执行回调 */

        printf("thread 0x%x end working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter_lock));
        pool->busy_thread_num--;
        pthread_mutex_unlock(&(pool->thread_counter_lock));

    }

    pthread_exit(NULL);

}

void *adjust_thread(void *threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;
    while(!pool->shutdown)
    {
        sleep(DEFAULT_TIME);    /* 定时 线程池管理周期 */

        pthread_mutex_lock(&(pool->my_lock));
        int queue_size = pool->queue_size;
        int live_thread_num = pool->live_thread_num;
        pthread_mutex_unlock(&(pool->my_lock));

        pthread_mutex_lock(&(pool->thread_counter_lock));
        int busy_thread_num = pool->busy_thread_num;
        pthread_mutex_unlock(&(pool->thread_counter_lock));

        /* 创建新线程 算法: 任务数大于最小线程池个数，且存活的线程数少于最大线程个数时 */
        if(queue_size >= MIN_WAIT_TASK_NUM && live_thread_num < pool->max_thread_num)
        {
            pthread_mutex_lock(&(pool->my_lock));
            int add = 0;

            /* 一次增加 DEFAULT_THREAD_VARY 个线程 */
            for(i = 0; i < pool->max_thread_num && add < DEFAULT_THREAD_VARY 
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
            pool->wait_exit_thread_num = DEFAULT_THREAD_VARY;
            pthread_mutex_unlock(&(pool->my_lock));

            for(i = 0; i < DEFAULT_THREAD_VARY; i++)
            {
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }      

    }

    return NULL;
    //pthread_exit(NULL);
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL)
    {
        return -1;
    }

    if(pool->task_queue)
    {
        free(pool->task_queue);
    }

    if(pool->threads)
    {
        free(pool->threads);
        pthread_mutex_lock(&(pool->my_lock));
        pthread_mutex_destroy(&(pool->my_lock));
        pthread_mutex_lock(&(pool->thread_counter_lock));
        pthread_mutex_destroy(&(pool->thread_counter_lock));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }

    free(pool);
    pool = NULL;
}

int threadpool_destroy(threadpool_t *pool)
{
    int i;
    if(pool == NULL)
    {
        return -1;
    }
    pool->shutdown = true;

    for(i = 0; i < pool->live_thread_num; i++)
    {
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    threadpool_free(pool);

    return 0;
}

int threadpool_all_threadnum(threadpool_t *pool)
{
    int all_threadnum = -1;

    pthread_mutex_lock(&(pool->my_lock));
    all_threadnum = pool->live_thread_num;
    pthread_mutex_unlock(&(pool->my_lock));

    return all_threadnum;
}

int threadpool_busy_threadnum(threadpool_t *pool)
{
    int busy_threadnum = -1;

    pthread_mutex_lock(&(pool->thread_counter_lock));
    busy_threadnum = pool->busy_thread_num;
    pthread_mutex_unlock(&(pool->thread_counter_lock));

    return busy_threadnum;
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