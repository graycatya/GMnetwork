#include "../../SrcLib/Threadpool/C++Catthreadpool/Threadpool.h"
#include<unistd.h>
using namespace CATTHREADPOOL;
using namespace std;

void* test(void* arg)
{
    int* args = (int *)arg; 
    cout << "arg " << args << endl;
    return nullptr;
}

int main()
{
    CatThreadPool th(20, 5, 10);
    th.AddThreads(test, (void*)1);
    //sleep(3);
    return 0;
}