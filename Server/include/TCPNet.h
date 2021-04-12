#ifndef _TCPNET_H
#define _TCPNET_H

#include "Thread_pool.h"
#include "packdef.h"
class IKernel
{
public:
    virtual void DealData(int,char*,int) = 0;
};

class TcpNet
{
public:
    TcpNet(IKernel* kernel){ m_kernel = kernel; }    int InitNetWork();
    void UnInitNetWork();
    static void *EPOLL_Jobs(void*);
    static void *Accept_Deal(void*);
    static void *Info_Recv(void*);
    int SendData(int,char*,int);
    void Addfd(int,int);
    void Deletefd(int);
    void Epoll_Deal(int,pool_t*);

    void SetpThis(TcpNet * tcp){ m_pThis = tcp ; }
public:
    struct epoll_event epollarr[_DEF_EPOLLSIZE];
    int sockfd;
    int clientfd;
    int epfd;

    pthread_mutex_t alock;
    pthread_mutex_t rlock;

    struct sockaddr_in serveraddr;
    IKernel * m_kernel;

    thread_pool * m_pool;
    static TcpNet * m_pThis;
};




#endif
