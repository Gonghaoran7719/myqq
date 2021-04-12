#ifndef TCPSELECTSERVER_H
#define TCPSELECTSERVER_H

#define FD_SETSIZE 1024
#include <process.h>
#include <winsock2.h>
#include<qmap>


class ITcpSelectServerMediator
{
public:
    virtual void DealData(unsigned int ip , char* szbuf , int nLen) = 0;
    virtual ~ITcpSelectServerMediator(){}
};

class TcpSelectServer
{
public:
    TcpSelectServer(ITcpSelectServerMediator *pMediator);
    ~TcpSelectServer();
public:
     bool InitNetWork(int port);
     void UnInitNetWork();
     int SendData(unsigned int ip ,char* szbuf,int nLen);
public:
    static  unsigned  __stdcall ThreadProc( void * );
    void ThreadProc();
    static unsigned int IPStringConvertToIPInt(char* IPString);
public:
    SOCKET m_MainSock;
    HANDLE m_hThread;
    bool   m_bFlagQuit;
    unsigned short m_port;
    ITcpSelectServerMediator *m_pMediator;
    int    m_nNumPro;
    QMap<unsigned int ,SOCKET> m_MapClientIPtoSocket;
    fd_set fdSocket;
    std::list<HANDLE> m_lstThread;
};

#endif //TCPSELECTSERVER_H
