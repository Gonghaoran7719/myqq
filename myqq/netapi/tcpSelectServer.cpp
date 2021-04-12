#include "tcpSelectServer.h"
#include<QHostAddress>
#include<QDebug>

TcpSelectServer::TcpSelectServer(ITcpSelectServerMediator *pMediator)
{
    m_MainSock  = NULL;
    m_nNumPro = 0;

    m_bFlagQuit = true;
    m_pMediator = pMediator;
}


TcpSelectServer::~TcpSelectServer(void)
{

}
bool TcpSelectServer::InitNetWork(int port)
{
     //0.选择类型 --- 加载库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {

        UnInitNetWork();
        return false;
    }


    m_MainSock = socket(AF_INET ,SOCK_STREAM,IPPROTO_TCP );
    if(INVALID_SOCKET == m_MainSock)
    {
        UnInitNetWork();
        return false;
    }
    sockaddr_in addr;
    addr.sin_addr.S_un.S_addr =INADDR_ANY;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    if( SOCKET_ERROR == bind(m_MainSock,(const sockaddr*)&addr,sizeof(addr)))
    {
        UnInitNetWork();
        return false;
    }

    SYSTEM_INFO  si;
    GetSystemInfo(&si);
    m_nNumPro = si.dwNumberOfProcessors*2;
    if(SOCKET_ERROR ==  listen(m_MainSock,m_nNumPro))
    {
        UnInitNetWork();
        return false;
    }
    // 初始化套节字集合fdSocket，添加监听套节字句柄到这个集合
    FD_ZERO(&fdSocket);
    FD_SET(m_MainSock, &fdSocket);

    HANDLE hThread= (HANDLE)_beginthreadex(0,0,&ThreadProc,this,0,0);
    if(!hThread)
    {
        UnInitNetWork();
        return false;
    }else
    {
        m_lstThread.push_back(hThread);
    }

    return true;
}

unsigned _stdcall TcpSelectServer::ThreadProc( void * lpvoid)
{
    TcpSelectServer *pthis = (TcpSelectServer *)lpvoid;
    pthis->ThreadProc();
    return 0;
}

void TcpSelectServer::ThreadProc()
{
    sockaddr_in socket_client;
    int nFromSize =sizeof(socket_client);
    int nPackSize = 0;
    int nRelReadNum;
    int noffset = 0;
    TIMEVAL tv;//设置超时等待时间
        tv.tv_sec = 1;
        tv.tv_usec = 0;
    while (m_bFlagQuit)
    {
        // 2）将fdSocket集合的一个拷贝fdRead传递给select函数，
        // 当有事件发生时，select函数移除fdRead集合中没有未决I/O操作的套节字句柄，然后返回。
        fd_set fdRead = fdSocket;
        int nRet = ::select(0, &fdRead, NULL, NULL, &tv);
        if (nRet > 0)
        {
            // 3）通过将原来fdSocket集合与select处理过的fdRead集合比较，
            // 确定都有哪些套节字有未决I/O，并进一步处理这些I/O。
            for (int i = 0; i < (int)fdSocket.fd_count; i++)
            {
                if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
                {
                    if (fdSocket.fd_array[i] == m_MainSock)        // （1）监听套节字接收到新连接
                    {
                        if (fdSocket.fd_count < FD_SETSIZE)
                        {
                            SOCKET socketWaiter = accept(m_MainSock,(sockaddr* )&socket_client ,&nFromSize);
                            //添加到集合中  socket_client.sin_addr.S_un.S_addr 是逆序的
                             qDebug()<< QHostAddress( htonl(socket_client.sin_addr.S_un.S_addr) ).toString()<<"client connect";
                            m_MapClientIPtoSocket[htonl(socket_client.sin_addr.S_un.S_addr)] = socketWaiter;
                            //添加到列表
                            FD_SET(socketWaiter, &fdSocket);
                        }
                        else
                        {
                            qDebug()<<" Too much connections!" ;
                            continue;
                        }
                    }else
                    {
                        nRelReadNum =::recv(fdSocket.fd_array[i], (char*)&nPackSize, sizeof(int), 0);
                        auto ite = m_MapClientIPtoSocket.begin();
                        for( ite; ite!= m_MapClientIPtoSocket.end() ; ++ite)
                        {
                            if( ite.value() == fdSocket.fd_array[i] )
                            {
                                break;
                            }
                        }
                        if( nRelReadNum <= 0 )
                        {
                            //将该socket 从map中移除
                            m_MapClientIPtoSocket.erase(ite);

                            ::closesocket(fdSocket.fd_array[i]);
                            FD_CLR(fdSocket.fd_array[i], &fdSocket);
                            continue;
                        }
                        char * pSzBuf = new char[nPackSize];
                        noffset = 0;
                        while(nPackSize)
                        {
                            nRelReadNum = ::recv(fdSocket.fd_array[i],pSzBuf+noffset,nPackSize , 0 );
                            if(nRelReadNum >0)
                            {
                                noffset += nRelReadNum;
                                nPackSize -= nRelReadNum;
                            }
                        }
                        this->m_pMediator->DealData( ite.key() , pSzBuf, nPackSize );
                    }
                }
            }
        }
        else
        {
            //qDebug()<<"Failed select()" ;
            continue;
        }
    }
}

unsigned int TcpSelectServer::IPStringConvertToIPInt(char* IPString)
{
    return inet_addr (IPString);
}


void TcpSelectServer::UnInitNetWork()
{
    //退出通知
    m_bFlagQuit = false;

     WSACleanup();
    if(m_MainSock)
    {
        closesocket(m_MainSock);
        m_MainSock = NULL;
    }

    auto ite = m_lstThread.begin();
    while(ite != m_lstThread.end())
    {
        if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
        {
            TerminateThread(*ite,-1);
        }
        CloseHandle(*ite);
        *ite = NULL;
        ite++;
    }
}

int  TcpSelectServer::SendData(unsigned int ip ,char* szbuf,int nLen)
{
    SOCKET sock = m_MapClientIPtoSocket[ip];

    if(  szbuf == nullptr||nLen <=0)
    {
        return -1;
    }
    //发数据前，先发送包大小
    if(send( sock,(const char*)&nLen,sizeof(int),0)<0)
    {
        return -1;
    }

    return send( sock,szbuf,nLen,0);

}
