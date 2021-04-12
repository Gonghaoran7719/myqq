#include "qmytcpselectserver.h"

QMyTcpSelectServer::QMyTcpSelectServer(QObject *parent) : QObject(parent)
{
    m_pServer = new TcpSelectServer(this);
}


QMyTcpSelectServer::~QMyTcpSelectServer()
{
    m_pServer->UnInitNetWork();
    if( m_pServer ) { delete m_pServer; m_pServer = 0; }
}

bool QMyTcpSelectServer::InitNetWork(int port)
{
    return m_pServer->InitNetWork(port);
}

void QMyTcpSelectServer::UnInitNetWork()
{
    m_pServer->UnInitNetWork();
}

int QMyTcpSelectServer::SendData(quint32 ip ,char* szbuf,int nLen )
{
    return m_pServer->SendData(ip,szbuf,nLen);
}

void QMyTcpSelectServer::DealData(unsigned int ip ,char* szbuf,int nLen)
{
    emit SIG_ReadyData( ip,szbuf, nLen);
}
