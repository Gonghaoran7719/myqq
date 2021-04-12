#include "udpnet.h"
#include"QDebug"
UdpNet::UdpNet(QObject * parent ):QObject(parent)
{
    OpenNet();
}

bool UdpNet::OpenNet() //资源的创建包括 网络库的加载 创建socket 及conn_info 打开线程 以及线程处理
{
    m_udpSocket = new QUdpSocket(this);
    port = DEF_UDP_PORT;
    m_udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));


    return true;
}

void UdpNet::CloseNet()
{
    if( m_udpSocket )
    m_udpSocket->close();
}

bool UdpNet::SendData(QByteArray data,quint32 IP )
{
    //qDebug( )<< data << "=="<< data.length();
    if ( m_udpSocket->writeDatagram(data,data.length(),QHostAddress(IP), port) )
    return true;
    return false;
}
bool UdpNet::SendBroadCast(QByteArray data)
{
    //qDebug( )<< data << "=="<< data.length();
    if ( m_udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port) )
    return true;
    return false;
}

bool UdpNet::SendBroadCast(QByteArray data,QString IP)
{
    //qDebug( )<< data << "=="<< data.length();
    if ( m_udpSocket->writeDatagram(data,data.length(),QHostAddress(IP), port) )
    return true;
    return false;
}

void UdpNet::processPendingDatagrams()
{
    while(m_udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress host;
        m_udpSocket->readDatagram(datagram.data(), datagram.size(),&host);

        emit SIG_RecvData(datagram , host.toIPv4Address());

    }
}
