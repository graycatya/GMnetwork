#include"Timeround.h"
#include<iostream>

using namespace std;

TimeWheel::TimeWheel()
{
    cout<<"TimeWheel()"<<endl;
    timeStamp.ms = timeStamp.s = timeStamp.min = 0;

    slot_ms=10;
    slot_s=60;
    slot_min=60;

    step = 100;
    callBackList = new list<Event>[slot_ms+slot_s+slot_min];
}

TimeWheel::~TimeWheel()
{
    delete[] callBackList;
}

void TimeWheel::Start()   //开启一个线程
{
    cout<<"Start()"<<endl;
    std::thread myThread([&]{
        this->DoLoop();
    });

    myThread.detach();
}

void TimeWheel::DoLoop()  //主循环
{
    cout<<"start"<<endl;
    while(true)
    {
        this_thread::sleep_for(chrono::milliseconds(step)); //让线程休息step毫秒的时间

        unique_lock<std::mutex> lock(_mutex);

        cout<<"time:"<<timeStamp.min <<" "<<timeStamp.s <<" "<<timeStamp.ms<<endl;
        TimeStamp per = timeStamp;        //per是原来的时间
        getNextTime(timeStamp,step);      //timeStamp向后走一步


        if(per.min!=timeStamp.min)  //分针有进位
        {
            //cout<<"(check min :" << slot_ms+slot_s+timeStamp.min <<")"<<endl;
            list<Event>& l = callBackList[slot_ms+slot_s+timeStamp.min];
            solvingEvents(l);
            l.clear();
        }
        else if(per.s!=timeStamp.s) //秒针有进位
        {
            //cout<<"(check s :" << slot_ms+timeStamp.s <<")"<<endl;
            list<Event>& l = callBackList[slot_ms+timeStamp.s];
            solvingEvents(l);
            l.clear();
        }
        else if(per.ms!=timeStamp.ms) //毫秒有进位
        {
            //cout<<"(check ms :" << timeStamp.ms <<")"<<endl;
            list<Event>& l = callBackList[timeStamp.ms];
            solvingEvents(l);
            l.clear();
        }

        lock.unlock();
    }
}

int TimeWheel::getMS(const TimeStamp &timeStamp)  //得到时间戳timeStamp一共多少ms
{
    return step * timeStamp.ms + timeStamp.s * 1000 + timeStamp.min * 60 * 1000;
}

void TimeWheel::solvingEvents(list<Event>l)
{
    for (auto item = l.begin(); item != l.end(); item++)
    {
        if(timeStamp == item->tri_time)  //触发时间到了
        {
            item->call_back();           //执行函数

            //如果需要发生多次加入下面两行
            getNextTime(item->tri_time,item->dur);
            InsertTimer(*item);
        }
        else
        {
            InsertTimer(*item);
        }
    }
}

void TimeWheel::getNextTime(TimeStamp &nowTimeStamp,int dur)//获得下一个时间
{
    int next_ms = getMS(nowTimeStamp)+dur;

    nowTimeStamp.min = next_ms/1000/60%slot_min;
    nowTimeStamp.s = (next_ms%(1000*60))/1000;
    nowTimeStamp.ms = (next_ms%1000)/step;
}

//添加定时器
int TimeWheel::AddTimer(int space,function<void(void)>& call_back)
{
    if(space<step)return -1;  //发生事件小于最小间隔
    if(space%step!=0)return -1;

    unique_lock<std::mutex> lock(_mutex);

    Event event;
    event.call_back = call_back;
    event.tri_time.ms = timeStamp.ms;
    event.tri_time.s = timeStamp.s;
    event.tri_time.min = timeStamp.min;
    event.dur = space;
    getNextTime(event.tri_time,event.dur);

    //cout<<"add a "<<space<<" clock"<<endl;
    cout<<event.tri_time.min<<":"<<event.tri_time.s<<":"<<event.tri_time.ms<<endl;
    InsertTimer(event);

    lock.unlock();
    return 0;
}

//先时间轮内插入事件
void TimeWheel::InsertTimer(Event &event)
{
    if(event.tri_time.min != timeStamp.min)   //分钟与现在不同的分钟就插入分的槽
        callBackList[slot_ms + slot_s + event.tri_time.min].push_back(event);
    else if(event.tri_time.s != timeStamp.s)
        callBackList[slot_ms + event.tri_time.s].push_back(event);
    else if(event.tri_time.ms != timeStamp.ms)
        callBackList[event.tri_time.ms].push_back(event);
}