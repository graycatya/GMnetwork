#ifndef _CATTIMEROUND_H_
#define _CATTIMEROUND_H_

#include <time.h>
#include <stdio.h>

#ifdef  __cplusplus  
extern "C" {  
#endif  

struct _timerevent_t
{
    int rotation; /* 记录定时器在时间轮转多少圈后生效 */
    int time_slot;  /* 记录定时器属于时间轮上那个槽 */
    void (*function) (void*); /* 定时器回调函数 */
    void* arg;  /* 回调参数 */
    struct _timerevent_t* next; /* 下一个 */
    struct _timerevent_t* prev; /* 前一个 */
};

struct _timerwheel_t
{
    int slot_n; /* 时间轮上槽的数目 */
    int si; /* 时间轮转动一次的时间 */
    struct _timerevent_t** slots;   /* 时间轮的槽，每个元素指向一个定时器链表，链表无序 */
    int current_slot;   /* 时间轮的当前槽 */
    int errno;  /* 错误码 */
};

typedef struct _timerevent_t timerevent_t;
typedef struct _timerwheel_t timerwheel_t;

timerwheel_t* Cat_timerwheel_create( int slot_n, int si );
int Cat_timerwheel_delete( timerwheel_t* timerwheel );
timerevent_t* Cat_add_timerevent(timerwheel_t* timerwheel, int timeout);
void Cat_del_timerevent(timerwheel_t* timerwheel, timerevent_t* timerevent);
void Cat_timerwheel_tick( timerwheel_t* timerwheel );

#ifdef  __cplusplus  
}  
#endif  /* end of __cplusplus */

#endif