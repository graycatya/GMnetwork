#include<cstdio>
#include<cstring>
#include"Timeround.h"
#include<iostream>

using namespace std;

void fun100()
{
    cout << "------------fun 100--------------" << endl;
}

void fun200()
{
    cout << "------------fun 200--------------" << endl;
}

void fun500()
{
    cout << "------------fun 500--------------" << endl;
}

void fun1500()
{
    cout << "------------fun 1500-------------" << endl;
}

int main()
{
    function<void(void)> f100 = std::bind(&fun100);
    function<void(void)> f200 = std::bind(&fun200);
    function<void(void)> f500 = std::bind(&fun500);
    function<void(void)> f1500 = std::bind(&fun1500);

    TimeWheel timeWheel;
    timeWheel.Start();

    //加入定时器
    timeWheel.AddTimer(10,f100);
    timeWheel.AddTimer(20,f200);
    timeWheel.AddTimer(50,f500);
    timeWheel.AddTimer(150,f1500);
    while(true){}
    return 0;
}