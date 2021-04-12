#ifndef TCPNET_H
#define TCPNET_H

#include <process.h>
#include <winsock2.h>

class ITcpMediator
{
public:
    virtual void DealData(char* szbuf , int nLen) = 0;
    virtual ~ITcpMediator(){}
};

class TCPNet
{
public:
    TCPNet(ITcpMediator *pMediator);
	~TCPNet();
public:
     bool InitNetWork(char* szBufIP , unsigned short port);

	 void UnInitNetWork();
     int SendData(char* szbuf,int nLen);
     int ConnectToServer( char* szBufIP);
public:
	static  unsigned  __stdcall ThreadProc( void * );
public:
	SOCKET m_sockClient;
	HANDLE m_hThread;
	bool   m_bFlagQuit;
    unsigned short m_port;
    ITcpMediator *m_pMediator;
};



#endif//TCPNET_H
