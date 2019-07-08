#include "CatTimeround.h"

int main()
{
    timerwheel_t* test = Cat_timerwheel_create(10, 1);
    Cat_timerwheel_delete(test);
    return 0;
}