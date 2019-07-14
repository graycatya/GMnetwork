#include "../../SrcLib/Threadpool/C++Catthreadpool/Threadpool.h"
#include<unistd.h>
using namespace CATTHREADPOOL;
using namespace std;

void* test(void* arg)
{
    int* args = (int *)arg; 
    cout << "arg " << (int)args << endl;
    return nullptr;
}

int main()
{
    CatThreadPool th(20, 5, 10);
    for(int i = 0; i < 100; i++)
    {
        th.AddThreads(test, (void*)i);
    }
    sleep(3);
    return 0;
}