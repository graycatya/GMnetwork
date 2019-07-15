#include <winsock2.h>
#include <stdio.h>
#include <iostream>

using namespace::std;
#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHanding(char * message);

typedef struct
{
    SOCKET hClntSock;   //套接字句柄
    char buf[BUF_SIZE]; //缓冲
    WSABUF wsaBuf;      //缓冲相关信息
}PER_IO_DATA,*LPPER_IO_DATA;


int main()
{
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    LPWSAOVERLAPPED lpOvlap;
    DWORD recvBytes;
    LPPER_IO_DATA hbInfo;
    int recvAdrSz;
    DWORD flagInfo = 0;
    u_long mode = 1;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//加载库并获取库信息填至wsaData
        ErrorHanding("socket start error!");

    //参数：协议族，套接字传输方式，使用的协议，WSA_PROTOCOL_INFO结构体地址/不需要时传null，扩展保留参数，套接字属性信息
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    //将hLisnSock句柄的套接字I/O模式(FIONBIO)改为mode中指定的形式:非阻塞模式
    ioctlsocket(hLisnSock, FIONBIO, &mode);

    //设置目标地址端口
    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
    lisnAdr.sin_port = htons(6000);

    //套接字绑定
    if (bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHanding("socket bind error!");

    //设置为监听模式
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHanding("socket listen error!");

    recvAdrSz = sizeof(recvAdr);
    while (1)
    {
        //进入短暂alertable wait 模式，运行ReadCompRoutine、WriteCompRoutine函数
        SleepEx(100, TRUE);

        //非阻塞套接字，需要处理INVALID_SOCKET
        //返回的新的套接字也是非阻塞的
        hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);
        if (hRecvSock == INVALID_SOCKET)
        {
            //无客户端连接时，accept返回INVALID_SOCKET，WSAGetLastError()返回WSAEWOULDBLOCK
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            else
                ErrorHanding("accept() error");
        }

        puts("Client connected");

        //申请重叠I/O需要使用的结构体变量的内存空间并初始化
        //在循环内部申请：每个客户端需要独立的WSAOVERLAPPED结构体变量
        lpOvlap = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvlap, 0, sizeof(WSAOVERLAPPED));

        hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->hClntSock = (DWORD)hRecvSock;

        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;

        //基于CR的重叠I/O不需要事件对象，故可以用来传递其他信息
        lpOvlap->hEvent = (HANDLE)hbInfo;
        //接收第一条信息
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvlap, ReadCompRoutine);
    }
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}
//参数：错误信息，实际收发字节数，OVERLAPPED类型对象，调用I/O函数时传入的特性信息
void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    //从lpoverlapped中恢复传递的信息
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;

    //接收到EOF，断开连接
    if (szRecvBytes == 0)
    {
        closesocket(hSock);
        free(hbInfo);
        free(lpOverlapped);
        puts("Client disconnected");
    }
    else
    {
        bufInfo->len = szRecvBytes;
        //将接收到的信息回传回去，传递完毕执行WriteCompRoutine(): 接收信息
        WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
    }
}

//参数：错误信息，实际收发字节数，OVERLAPPED类型对象，调用I/O函数时传入的特性信息
void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
    //从lpoverlapped中恢复传递的信息
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes;
    DWORD flagInfo = 0;
    //接收数据，接收完毕执行ReadCompRoutine：发送数据
    WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

void ErrorHanding(char * message)
{
    cout << message << endl;
    exit(1);
}