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
    timerwheel->slots = (timerevent_t*)malloc(sizeof(timerevent_t) * slot_n);
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
}

timerevent_t* Cat_add_timerevent(int timeout)
{

}

void Cat_del_timerevent(timerevent_t* timerevent)
{

}

void Cat_timerwheel_tick( timerwheel_t* timerwheel, timerevent_t* timerevent )
{

}
