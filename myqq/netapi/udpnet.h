#ifndef UDPNET_H
#define UDPNET_H

#include <QUdpSocket>
#include <QObject>

#define DEF_UDP_PORT 2345
class UdpNet:public QObject
{
    Q_OBJECT
public:
    explicit UdpNet(QObject* parent = 0 );

     ~UdpNet(){ CloseNet(); }
    virtual bool OpenNet( ); //资源的创建包括 网络库的加载 创建socket 及conn_info 打开线程 以及线程处理
    virtual void CloseNet();
    virtual bool SendData(QByteArray data,quint32  IP );
    bool SendBroadCast(QByteArray data);
    bool SendBroadCast(QByteArray data,QString IP);
private:
    QUdpSocket * m_udpSocket;
    unsigned int port;

private slots:
    void processPendingDatagrams();
signals:
    void SIG_RecvData( QByteArray arr ,quint32 ip );
};


#endif // UDPNET_H
