#ifndef TIMEROUND_H
#define TIMEROUND_H

#include<functional>
#include<list>
#include<thread>
#include<mutex>

struct TimeStamp //时间戳
{
    int ms;
    int s;
    int min;
    bool operator==(TimeStamp timeStamp)
    {
        return timeStamp.ms==ms&&timeStamp.s==s&&timeStamp.min==min;
    }
};

struct Event     //挂在时间槽上的事件
{
    std::function<void(void)> call_back;
    TimeStamp tri_time;    //事件触发的时间
    int dur;               //多久触发
};

class TimeWheel  //时间轮
{
public:
    TimeWheel();
    ~TimeWheel();

    void Start();  //启动
    int AddTimer(int space,std::function<void(void)>& call_back); //添加定期器

private:
    void DoLoop();//主循环
    void InsertTimer(Event &event);  //插入事件到时间槽
    int getMS(const TimeStamp &timeStamp);
    void solvingEvents(std::list<Event>l); //解决时间槽上的事件
    void getNextTime(TimeStamp &nowTimeStamp,int dur);  //得到下个时间

private:
    std::list<Event> *callBackList = nullptr;  //当做时间槽上，每一个槽都是一个链表
    std::mutex _mutex;  //互斥量

    TimeStamp timeStamp;  //事件轮的时间

    int slot_ms;     //毫秒的时间槽数量
    int slot_s;      //秒的时间槽数量
    int slot_min;    //分的时间槽数量

    int step;        //最小的间隔时间
};

#endif