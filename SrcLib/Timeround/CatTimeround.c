#include "CatTimeround.h"

timerwheel_t* Cat_timerwheel_create( int slot_n, int si )
{
    timerwheel_t* timerwheel = (timerwheel_t*)malloc(sizeof(timerwheel_t));
    if(slot_n <= 0 || si <= 0)
    {
        timerwheel->errno = -1;
        return timerwheel;
    }
    timerwheel->slot_n = slot_n;
    timerwheel->si = si;
    timerwheel->current_slot = 0;
    timerwheel->slots = (timerevent_t**)malloc(sizeof(timerevent_t) * slot_n);
    timerwheel->errno = 0;
    return timerwheel;
}

int Cat_timerwheel_delete( timerwheel_t* timerwheel )
{
    if(timerwheel != NULL)
    {
        for(int i = 0; i < timerwheel->slot_n; i++)
        {
            if(timerwheel->slots != NULL)
            {
                if(timerwheel->slots[i] != NULL)
                {
                    free(timerwheel->slots[i]);
                    timerwheel->slots[i] = NULL;
                }
                free(timerwheel->slots);
                timerwheel->slots = NULL;
            }
        }
        free(timerwheel);
        timerwheel = NULL;
    }
    else
    {
        return -1;
    }
    
    return 0;
}

timerevent_t* Cat_add_timerevent(timerwheel_t* timerwheel, int timeout)
{
    if(timeout < 0 || timerwheel == NULL)
    {
        return NULL;
    }
    int ticks = 0;
    if(timeout < timerwheel->si)
    {
        ticks = 1;
    }
    else
    {
        ticks = timeout / timerwheel->si;
    }
    /* 计算待插入的定时器在时间转动多少圈后被触发 */
    int rotation = 0;
    rotation = ticks / timerwheel->slot_n;

    /* 计算待插入的定时器应该被插入哪个槽中 */
    int ts = ( timerwheel->current_slot + ticks) % timerwheel->slot_n;
    /* 创建新的定时器，它在时间轮转动rotation圈之后被触发，且位于第ts个槽上 */
    timerevent_t* timer = (timerevent_t*)malloc(sizeof(timerevent_t));
    
    timer->rotation = rotation;
    timer->time_slot = ts;
    /* 如果第ts个槽中尚无任何定时器，则把新建的定时器插入其中，并将该定时器设置为该槽的头结点 */
    if(! timerwheel->slots[ts] )
    {
        printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n", timer->rotation, ts, timerwheel->current_slot);
        timerwheel->slots[ts] = timer;
    }
    else
    {
        /* 新结点插入头结点 */
        timer->next = timerwheel->slots[ts];
        timerwheel->slots[ts]->prev = timer;
        timerwheel->slots[ts] = timer;
    }
    
    return timer;
}

void Cat_del_timerevent(timerwheel_t* timerwheel, timerevent_t* timerevent)
{
    if(timerevent == NULL)
    {
        return;
    }
    int ts = timerevent->time_slot;
    /* slots[ts]是目标定时器所在槽的头结点，如果目标定时器就是该头结点，则需要重置第ts个槽的头结点 */
    if(timerevent == timerwheel->slots[ts])
    {
        timerwheel->slots[ts] = timerwheel->slots[ts]->next;
        if(timerwheel->slots[ts])
        {
            timerwheel->slots[ts]->prev = NULL;
        }
        free(timerevent);
    }
    else
    {
        timerevent->prev->next = timerevent->next;
        if(timerevent->next)
        {
            timerevent->next->prev = timerevent->prev;
        }
        free(timerevent);
    }
}

void Cat_timerwheel_tick( timerwheel_t* timerwheel )
{
    /* 取得时间轮上当前槽的头结点 */
    timerevent_t* timerevent = timerwheel->slots[timerwheel->current_slot];
    printf("current slot is %d\n", timerwheel->current_slot);
    while(timerevent)
    {
        printf( "tick the timer once\n" );
        /* 如果定时器的rotation值大于0，则它在这一轮不起作用 */
        if(timerevent->rotation > 0)
        {
            timerevent->rotation = timerevent->rotation - 1;
            timerevent = timerevent->next;
        }
        /* 否则，说明定时器已经到期，于是执行定时任务，然后删除该定时器 */
        else
        {
            /* code */
            timerevent->function( timerevent->arg );
            if(timerevent == timerwheel->slots[timerwheel->current_slot])
            {
                printf( "delete header in cur_slot\n" );
                timerwheel->slots[timerwheel->current_slot] = timerevent->next;
                free(timerevent);
                if(timerwheel->slots[timerwheel->current_slot])
                {
                    timerwheel->slots[timerwheel->current_slot]->prev = NULL;
                }
                timerevent = timerwheel->slots[timerwheel->current_slot];
            }
            else
            {
                timerevent->prev->next = timerevent->next;
                if(timerevent->next)
                {
                    timerevent->next->prev = timerevent->prev;
                }
                timerevent_t* timerevent2 = timerevent->next;
                free(timerevent);
                timerevent = timerevent2;
            }
            
        }
    }
    /* 时间轮转动 */
    timerwheel->current_slot = (timerwheel->current_slot+1) % timerwheel->slot_n;
}
