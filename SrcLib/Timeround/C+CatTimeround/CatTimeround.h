#pragma once

namespace CATTIMERROUND
{

class TimerEvent
{
    public:
        TimerEvent(int rotation, int time_slot) 
            : next(nullptr), prev(nullptr),
                rotation(rotation), time_slot(time_slot)
        {}
    public:
        int rotation; /* 记录定时器在时间轮转多少圈后生效 */
        int time_slot;  /* 记录定时器属于时间轮上那个槽 */
        void (*function) (void*); /* 定时器回调函数 */
        void* arg;  /* 回调参数 */
        TimerEvent* next; /* 下一个 */
        TimerEvent* prev; /* 前一个 */
};

class TimerWheel
{
    public:
        TimerWheel(int slot_n, int si);
        ~TimerWheel();
        TimerEvent* add(int timeout);
        void del(TimerEvent* timerevent);
        void tick();

    private:
        int slot_n; /* 时间轮上槽的数目 */
        int si; /* 时间轮转动一次的时间 */
        struct TimerEvent** slots;   /* 时间轮的槽，每个元素指向一个定时器链表，链表无序 */
        int current_slot;   /* 时间轮的当前槽 */  
};



}