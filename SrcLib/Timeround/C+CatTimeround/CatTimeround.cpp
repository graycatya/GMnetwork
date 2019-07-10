#include "CatTimeround.h"



CATTIMERROUND::TimerWheel::TimerWheel(int slot_n, int si)
    : slot_n(slot_n), si(si), current_slot(0)
{
    if(slot_n <= 0 || si <= 0)
    {
        this->slot_n = 5;
        this->si = 1;
    }
    this->slots = new TimerEvent*[this->slot_n];
    for(int i = 0; i < this->slot_n; i++)
    {
        this->slots[i] = nullptr;
    }
}

CATTIMERROUND::TimerWheel::~TimerWheel()
{
    for(int i = 0; i < this->slot_n; i++)
    {
        TimerEvent* temp = this->slots[i];
        while(temp != nullptr)
        {
            this->slots[i] = temp->next;
            delete temp;
            temp = this->slots[i];
        }
    }
}

CATTIMERROUND::TimerEvent* CATTIMERROUND::TimerWheel::add(int timeout)
{
    if(timeout < 0) 
    {
        return nullptr;
    }
    int ticks = 0;
    if(timeout < si)
    {
        ticks = 1;
    }
    else
    {
        ticks = timeout / si;
    }

    int rotation = 0;
    rotation = ticks / slot_n;

    int ts = (current_slot + ticks) % slot_n;

    TimerEvent* temp = new TimerEvent(rotation, ts);
    if(!slots[ts])
    {
        slots[ts] = temp;
    }
    else
    {
        temp->next = slots[ts];
        slots[ts]->prev = temp;
        slots[ts] = temp;
    }
    
    return temp;
}

void CATTIMERROUND::TimerWheel::del(TimerEvent* timerevent)
{
    if( !timerevent )
    {
        return;
    }
    int ts = timerevent->time_slot;
    if(timerevent == slots[ts])
    {
        slots[ts] = slots[ts]->next;
        if(slots[ts])
        {
            slots[ts]->prev = nullptr;
        }
        delete timerevent;
    }
    else
    {
        /* code */
        timerevent->prev->next = timerevent->next;
        if(timerevent->next)
        {
            timerevent->next->prev = timerevent->prev;
        }
        delete timerevent;
    }
}

void CATTIMERROUND::TimerWheel::tick()
{
    TimerEvent* temp = slots[current_slot];
    while(temp)
    {
        if(temp->rotation > 0)
        {
            temp->rotation--;
            temp = temp->next;
        }
        else
        {
            temp->function(temp->arg);
            if(temp == slots[current_slot])
            {
                slots[current_slot] = temp->next;
                delete temp;
                if(slots[current_slot])
                {
                    slots[current_slot]->prev = nullptr;
                }
                temp = slots[current_slot];
            }
            else
            {
                temp->prev->next = temp->next;
                if(temp->next)
                {
                    temp->next->prev = temp->prev;
                }
                TimerEvent* temp2 = temp->next;
                delete temp;
                temp = temp2;
            }
        }
    }
    current_slot = ++current_slot % slot_n;
}
