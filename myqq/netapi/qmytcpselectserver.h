#ifndef QMYTCPSELECTSERVER_H
#define QMYTCPSELECTSERVER_H

#include <QObject>
#include"tcpselectserver.h"

#define DEFAULT_TCP_PORT 1234
class QMyTcpSelectServer : public QObject, public ITcpSelectServerMediator
{
    Q_OBJECT
public:
    explicit QMyTcpSelectServer(QObject *parent = 0);
    ~QMyTcpSelectServer();
signals:
    SIG_ReadyData(unsigned int ip ,char* szbuf,int nLen);
public slots:
    bool InitNetWork(int port) ;
    void UnInitNetWork();
    int SendData( quint32 ip ,char* szbuf,int nLen);

    virtual void DealData( unsigned int ip ,char* szbuf,int nLen);
private:
    TcpSelectServer *m_pServer;
};


#endif // QMYTCPSELECTSERVER_H
