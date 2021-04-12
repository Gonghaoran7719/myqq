#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H



#include "TCPNet.h"
#include "Mysql.h"
#include<map>

class TcpKernel;
typedef void (TcpKernel::*PFUN)(int,char*,int nlen);

typedef struct
{
    PackType m_type;
    PFUN m_pfun;
} ProtocolMap;



class TcpKernel:public IKernel
{
public:
    int Open();
    void Close();
    void DealData(int,char*,int);

    //注册
    void RegisterRq(int,char*,int);
    //登录
    void LoginRq(int,char*,int);

    //从数据库获取个人信息
    UserInfo* getUserInfoFromSql( int id);
    //用户列表 : 将 id = id的用户的好友发给用户, 并且把这个人的信息再发给好友
    void SendUserList(int id);

    void AddFriendRq(int clientfd, char *szbuf, int nlen);
    void AddFriendRs(int clientfd, char *szbuf, int nlen);
    void SendMsgToOnlineClient( int id , char *szbuf, int nlen);
    void ChatRq(int clientfd, char *szbuf, int nlen);
    void CreateRoomRq(int clientfd, char *szbuf, int nlen);
    void JoinRoomRq(int clientfd, char *szbuf, int nlen);
    void LeaveRoomRq(int clientfd, char *szbuf, int nlen);
    void AudioFrameRq(int clientfd, char *szbuf, int nlen);
    void VideoFrameRq(int clientfd, char *szbuf, int nlen);
    void InitRandom();
    void OfflineRq(int clientfd, char *szbuf, int nlen);
private:
    CMysql * m_sql;
    TcpNet * m_tcp;

    map<int , UserInfo*> m_mapIDToUserInfo;
    map<int , list<UserInfo*> > m_mapRoomIDToUserList;
};

#endif
