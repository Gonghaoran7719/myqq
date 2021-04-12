#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>
#include<sys/time.h>

using namespace std;


////注册
//#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
//#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)
////登录
//#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
//#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)

static const ProtocolMap m_ProtocolMapEntries[] =
{
    {DEF_PACK_REGISTER_RQ , &TcpKernel::RegisterRq},
    {DEF_PACK_LOGIN_RQ , &TcpKernel::LoginRq},
    {DEF_PACK_ADD_FRIEND_RQ , &TcpKernel::AddFriendRq},
    {DEF_PACK_ADD_FRIEND_RS , &TcpKernel::AddFriendRs},
    {DEF_PACK_CHAT_RQ , &TcpKernel::ChatRq},
    {DEF_PACK_CREATEROOM_RQ , &TcpKernel::CreateRoomRq},
    {DEF_PACK_JOINROOM_RQ , &TcpKernel::JoinRoomRq},
    {DEF_PACK_LEAVEROOM_RQ , &TcpKernel::LeaveRoomRq},
    {DEF_PACK_OFFLINRE_RQ, &TcpKernel::OfflineRq},
    {DEF_PACK_AUDIO_FRAME , &TcpKernel::AudioFrameRq},
    {DEF_PACK_VIDEO_FRAME , &TcpKernel::VideoFrameRq},
    {0,0}
};
#define RootPath   "/home/colin/Video/"


int TcpKernel::Open()
{
    InitRandom();
    m_sql = new CMysql;
    m_tcp = new TcpNet(this);
    m_tcp->SetpThis(m_tcp);
    pthread_mutex_init(&m_tcp->alock,NULL);
    pthread_mutex_init(&m_tcp->rlock,NULL);
    if(  !m_sql->ConnectMysql("localhost","root","colin123","myqq")  )
    {
        printf("Conncet Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    if( !m_tcp->InitNetWork()  )
    {
        printf("InitNetWork Failed...\n");
        return FALSE;
    }
    else
    {
        printf("Init Net Success...\n");
    }

    return TRUE;
}

void TcpKernel::Close()
{
    for( auto ite = m_mapIDToUserInfo.begin();ite!=m_mapIDToUserInfo.end();++ite)
    {
        delete ite ->second;
    }
    m_mapIDToUserInfo.clear();
    m_sql->DisConnect();
    m_tcp->UnInitNetWork();
}


void TcpKernel::DealData(int clientfd,char *szbuf,int nlen)
{
    PackType *pType = (PackType*)szbuf;
    int i = 0;
    while(1)
    {
        if(*pType == m_ProtocolMapEntries[i].m_type)
        {
            auto fun= m_ProtocolMapEntries[i].m_pfun;
            (this->*fun)(clientfd,szbuf,nlen);
        }
        else if(m_ProtocolMapEntries[i].m_type == 0 &&
                m_ProtocolMapEntries[i].m_pfun == 0)
            return;
        ++i;
    }
    return;
}
//添加好友申请
void TcpKernel::AddFriendRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d AddFriendRq\n", clientfd);
    //解析
    STRU_ADD_FRIEND_RQ* rq = (STRU_ADD_FRIEND_RQ*)szbuf;
    STRU_ADD_FRIEND_RS rs;
    rs.m_userID = rq->m_userID;
    strcpy(rs.szAddFriendName , rq->m_szAddFriendName);
    //查数据库, 找id 没有人 , 返回结果
    char sqlbuf[_DEF_SQLIEN] = "";
    sprintf(sqlbuf , "select id from t_user where name='%s'",rq->m_szAddFriendName);
    list<string> resID;
    m_sql->SelectMysql(sqlbuf, 1, resID);
    if( resID.size() > 0 )
    {
        int id = atoi(resID.front().c_str() );
        //根据id看是否在线
        if(m_mapIDToUserInfo.find(id) == m_mapIDToUserInfo.end() )
        {//不在线 返回结果
            rs.m_result = user_is_offline;
        }else
        {//在线 转发
            SendMsgToOnlineClient( id , szbuf , nlen);
            return;
        }
    }else
    {
        rs.m_result = no_this_user;

    }
    m_tcp->SendData(clientfd , (char*)&rs, sizeof(rs));

}

void TcpKernel::InitRandom()
{
    //时间做随机种子
    struct timeval time;
    gettimeofday( &time , NULL);
    srand( time.tv_sec + time.tv_usec);
}
//创建房间请求
void TcpKernel::CreateRoomRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d CreateRoomRq\n", clientfd);
    //解析
    STRU_CREATEROOM_RQ*rq = (STRU_CREATEROOM_RQ*)szbuf;
 //   rq->m_UserID;
    //id->人不在线 不管
    if( m_mapIDToUserInfo.find(rq->m_UserID ) == m_mapIDToUserInfo.end()) return;
    UserInfo* user = m_mapIDToUserInfo[rq->m_UserID];
    //roomid 随机 --> 如果0 或者重复 , 重新随机
    int roomid = 0;
    do
    {
        roomid = rand()%1000000;
    }while(roomid == 0 || ( m_mapRoomIDToUserList.find(roomid) != m_mapRoomIDToUserList.end() ) );
    STRU_CREATEROOM_RS rs;
    rs.m_RoomId = roomid;
    rs.m_lResult = 1;
    //存储在map
    //创建者添加到list --> map
    list<UserInfo*> lst;
    lst.push_back(user);
    user->m_roomID = roomid;
    m_mapRoomIDToUserList[roomid] = lst; // 浅拷贝 -- > 拷贝构造
    //回复包
    m_tcp->SendData( clientfd, (char*)&rs, sizeof(rs));
}
//加入房间请求
void TcpKernel::JoinRoomRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d JoinRoomRq\n", clientfd);
    //解析
    STRU_JOINROOM_RQ* rq = (STRU_JOINROOM_RQ* )szbuf;
    STRU_JOINROOM_RS rs;

    //加入者的信息 不在线 退出->结果
    if( m_mapIDToUserInfo.find(rq->m_UserID) == m_mapIDToUserInfo.end() )
    {
        rs.m_lResult = 0 ;
        m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs));
        return;
    }
    UserInfo * joiner =  m_mapIDToUserInfo[rq->m_UserID];
    //加入房间 拿到list 查不到房间 结果失败
    if(m_mapRoomIDToUserList.find(rq->m_RoomID) == m_mapRoomIDToUserList.end())
    {
        rs.m_lResult = 0 ;
        m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs));
        return;
    }

    joiner->m_roomID = rq->m_RoomID;
    STRU_ROOM_MEMBER_RQ JoinRq;
    JoinRq.m_UserID = joiner->m_id;
    strcpy( JoinRq.m_szUser , joiner->m_userName);

    //拿到list
    list<UserInfo*> lst = m_mapRoomIDToUserList[rq->m_RoomID];
    rs.m_lResult = 1;
    rs.m_roomID = rq->m_RoomID;
    //发送回复结果
    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs));

    //遍历list
    for( auto ite = lst.begin() ; ite!= lst.end(); ++ite)
    {
        UserInfo* inner = *ite;

        STRU_ROOM_MEMBER_RQ InnerRq;
        InnerRq.m_UserID = inner->m_id;
        strcpy( InnerRq.m_szUser , inner->m_userName);

        //发送加入者信息给房间每个人
        m_tcp->SendData(inner->m_fd , (char*)&JoinRq , sizeof(JoinRq));
        //发送房间人的信息给加入者
        m_tcp->SendData(joiner->m_fd , (char*)&InnerRq , sizeof(InnerRq));
    }

    //添加到list
    lst.push_back(joiner);
    //更新map
    m_mapRoomIDToUserList[rq->m_RoomID] = lst;
}
//离开房间请求
void TcpKernel::LeaveRoomRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d LeaveRoomRq\n", clientfd);
    //解析
    STRU_LEAVEROOM_RQ *rq = (STRU_LEAVEROOM_RQ *)szbuf;

    //找用户信息  map
    if( m_mapIDToUserInfo.find(rq->m_nUserId) ==m_mapIDToUserInfo.end() )  return;
    UserInfo* leaver = m_mapIDToUserInfo[rq->m_nUserId];
    //找房间信息 map
    if( m_mapRoomIDToUserList.find(rq->m_RoomId) == m_mapRoomIDToUserList.end()) return;
    list<UserInfo*> lst = m_mapRoomIDToUserList[rq->m_RoomId];//找到  拿到list

    leaver->m_roomID = 0;
    STRU_LEAVEROOM_RS rs;
    rs.m_UserID = rq->m_nUserId;
    strcpy( rs.szUserName , leaver->m_userName);

    //遍历list
    auto ite = lst.begin();
    while(ite != lst.end())
    {
        UserInfo * inner = *ite;
        //发给每一个用户 离开人的信息
        if( inner->m_id != rq->m_nUserId )
        {
            this->SendMsgToOnlineClient(inner->m_id , (char*)&rs,sizeof(rs) );
            ++ite;
        }else
        {//从list移除这个节点
            ite = lst.erase(ite);
        }
    }
    //更新map
    m_mapRoomIDToUserList[ rq->m_RoomId ] = lst;
}
//音频真
void TcpKernel::AudioFrameRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d AudioFrameRq\n", clientfd);

    char* tmp = szbuf;
    tmp += sizeof(int);
    int userId = *(int*)tmp;
    tmp += sizeof(int);
    int roomId = *(int*)tmp;
    tmp += sizeof(int);

    if( m_mapRoomIDToUserList.find(roomId) != m_mapRoomIDToUserList.end())
    {
        list<UserInfo*> lst = m_mapRoomIDToUserList[roomId];
        //遍历链表
        for(  auto ite = lst.begin() ; ite != lst.end() ; ++ite )
        {
            UserInfo* user = *ite;
            if( user->m_id != userId )
            {
                this->SendMsgToOnlineClient(user->m_id ,szbuf , nlen );
            }
        }
    }
}
//视频帧
void TcpKernel::VideoFrameRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d VideoFrameRq\n", clientfd);
    ///视频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray videoFrame;

    char* tmp = szbuf;
    tmp += sizeof(int);
    int userId = *(int*)tmp;
    tmp += sizeof(int);
    int roomId = *(int*)tmp;
    tmp += sizeof(int);

    if( m_mapRoomIDToUserList.find(roomId) != m_mapRoomIDToUserList.end())
    {
        list<UserInfo*> lst = m_mapRoomIDToUserList[roomId];
        //遍历链表
        for(  auto ite = lst.begin() ; ite != lst.end() ; ++ite )
        {
            UserInfo* user = *ite;
            if( user->m_id != userId )
            {
                this->SendMsgToOnlineClient(user->m_id ,szbuf , nlen );
            }
        }
    }
}





//聊天请求
void TcpKernel::ChatRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d ChatRq\n", clientfd);
    //解析
    STRU_CHAT_RQ* rq = (STRU_CHAT_RQ* )szbuf;
    //是否在线
    if( m_mapIDToUserInfo.find( rq->m_friendID ) == m_mapIDToUserInfo.end() )
    {//不在线 写rs
        STRU_CHAT_RS rs;
        rs.m_result = user_is_offline;
        rs.m_userID = rq->m_userID;
        rs.m_friendID = rq->m_friendID;
        m_tcp->SendData(clientfd , (char*)&rs , sizeof(rs));
    }else
    {//在线 转发
        SendMsgToOnlineClient( rq->m_friendID  ,szbuf,nlen );
    }
}

////添加好友结果
//#define no_this_user    0
//#define user_refused    1
//#define user_is_offline 2
//#define add_success     3

void TcpKernel::AddFriendRs(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d AddFriendRs\n", clientfd);
    //解析
    STRU_ADD_FRIEND_RS *rs = (STRU_ADD_FRIEND_RS *)szbuf;
    if( rs->m_result  == add_success )//查看结果  成功 更新数据库 , 发好友列表
    {
        char sqlbuf[_DEF_SQLIEN]="";
        sprintf(sqlbuf, "insert into t_friend(idA , idB) values( %d , %d);", rs->m_userID, rs->m_friendID);
        m_sql->UpdataMysql(sqlbuf);

        sprintf(sqlbuf, "insert into t_friend(idA , idB) values( %d , %d);", rs->m_friendID ,rs->m_userID);
        m_sql->UpdataMysql(sqlbuf);

        getUserInfoFromSql(rs->m_userID);
        SendUserList(rs->m_userID);
    }
    //转发
    SendMsgToOnlineClient( rs->m_userID , szbuf , nlen);
}
//转发给在线用户
void TcpKernel::SendMsgToOnlineClient(int id, char *szbuf, int nlen)
{
    if( m_mapIDToUserInfo.find(id) != m_mapIDToUserInfo.end())
    {
        m_tcp->SendData( m_mapIDToUserInfo[id]->m_fd , szbuf, nlen);
    }
}
//注册
void TcpKernel::RegisterRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d RegisterRq\n", clientfd);

    STRU_REGISTER_RQ * rq = (STRU_REGISTER_RQ *)szbuf;
    STRU_REGISTER_RS rs;

    char sqlBuf[_DEF_SQLIEN] ="";
    sprintf( sqlBuf , "select name from t_user where name= '%s';", rq->m_szUser);
    list<string> resList;
    bool res = m_sql->SelectMysql( sqlBuf ,1 , resList);
    if(!res )
    {
        cout<< "SelectMysql error: "<<sqlBuf <<endl;
        return;
    }
//    //注册请求结果
//    #define userid_is_exist      0
//    #define register_sucess      1
    //查表 查name
    if( resList.size() > 0 )
    {
        //有  不成功
        rs.m_lResult = userid_is_exist;
    }else
    {//没有 写表 成功
        char sqlBuf[_DEF_SQLIEN] ="";
        sprintf( sqlBuf , "insert into t_user (name ,password) values('%s','%s');",
                 rq->m_szUser,rq->m_szPassword);
        m_sql->UpdataMysql(sqlBuf);

        sprintf( sqlBuf , "select id from t_user where name ='%s'",rq->m_szUser);
        list<string > resID;
        m_sql->SelectMysql(sqlBuf , 1,resID );
        int id = 0;
        if( resID.size() > 0)
        {
            id = atoi( resID.front().c_str() );
        }
        //插入用户信息
        sprintf( sqlBuf , "insert into t_userInfo (id , name ,icon , feeling) values(%d , '%s' , %d , '%s');",
                id, rq->m_szUser,0,"");
        m_sql->UpdataMysql(sqlBuf);

        rs.m_lResult = register_sucess;
    }

    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
}
//登录
void TcpKernel::LoginRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d LoginRq\n", clientfd);

    STRU_LOGIN_RQ * rq = (STRU_LOGIN_RQ *)szbuf;
    STRU_LOGIN_RS rs;
//    //登录请求结果
//    #define userid_no_exist      0
//    #define password_error       1
//    #define login_sucess         2
//    #define user_online          3
    char buf[_DEF_SQLIEN] = "";
    sprintf(buf, "select password ,id from t_user where name = '%s';", rq->m_szUser);
    list<string> resList;
    bool res = m_sql->SelectMysql(buf , 2 , resList);
    if( !res )
    {
        cout<< "SelectMysql errot" << buf <<endl;
    }
    if( resList.size() > 0 )
    {

        if(  strcmp( resList.front().c_str()  ,rq->m_szPassword ) == 0  )
        {
            rs.m_lResult = login_sucess;
            resList.pop_front();
            rs.m_UserID = atoi(resList.front().c_str());

            m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );

            if( m_mapIDToUserInfo.find(rs.m_UserID) == m_mapIDToUserInfo.end() )
            {//没找到
                //新建
                UserInfo * info = new UserInfo;
                info->m_fd = clientfd;
                info->m_id = rs.m_UserID;
                strcpy( info->m_userName , rq->m_szUser);
                info->m_state = 1;

                m_mapIDToUserInfo[rs.m_UserID ] = info;

            }else
            {//找到
                //发强制下线
                STRU_FORCE_OFFLINE off;
                off.m_UserID = rs.m_UserID;
                UserInfo * info = m_mapIDToUserInfo[rs.m_UserID];
                m_tcp->SendData( info->m_fd , (char*)&off,sizeof(off) );
                //改info
                info->m_fd = clientfd;
                //存map
                m_mapIDToUserInfo[rs.m_UserID] = info;
            }
            //好友列表
            getUserInfoFromSql( rs.m_UserID);
            SendUserList(rs.m_UserID);
            //发送上线的通知

            //存储映射关系
            //m_mapNameToFD[ rq->m_szUser ] = clientfd;
            return;
        }else
        {
            rs.m_lResult = password_error;
        }
    }else
    {
        rs.m_lResult = userid_no_exist;
    }

    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );

}

//从数据库获取个人信息
UserInfo* TcpKernel::getUserInfoFromSql( int id)
{
     if( m_mapIDToUserInfo.find( id ) == m_mapIDToUserInfo.end()) return NULL;
     UserInfo* info = m_mapIDToUserInfo[id];
     //从数据库里面取 更新info 再返回
     char sqlbuf[_DEF_SQLIEN ]="";
     sprintf(sqlbuf,"select name, icon , feeling from t_userInfo where id = %d",id);
     list<string> res;
     m_sql->SelectMysql( sqlbuf , 3, res);
     if( res.size() > 0 )
     {
         strcpy(info->m_userName, res.front().c_str());
         res.pop_front();
         info->m_iconID = atoi(res.front().c_str());
         res.pop_front();
         strcpy(info->m_feeling, res.front().c_str());
         res.pop_front();
     }
     return info;
}
//用户列表 : 将 id = id的用户的好友发给用户, 并且把这个人的信息再发给好友
void TcpKernel::SendUserList(int id)
{
    //先找该用户
    if( m_mapIDToUserInfo.find( id ) == m_mapIDToUserInfo.end()) return;
    //写用户信息 loginer  用户信息请求loginRq
    UserInfo *loginer =  m_mapIDToUserInfo[id];

    STRU_FRIEND_INFO loginRq;
    loginRq.m_state = 1;
    loginRq.m_iconID = loginer->m_iconID;
    strcpy(  loginRq.m_szName , loginer->m_userName);
    strcpy(loginRq.m_feeling , loginer->m_feeling);
    loginRq.m_userID= loginer->m_id;

    char sqlbuf[_DEF_SQLIEN ] = "";
    sprintf(sqlbuf,"select idB from t_friend where idA = %d",id );
    list<string> listID;

     m_sql->SelectMysql( sqlbuf, 1, listID );
     if( listID .size() == 0) return;
     //查所有好友列表  friender   用户信息请求 frienderRq
     //好友信息发给登录的人 , 登录的人的信息发给每个好友
     for( auto ite = listID.begin() ; ite != listID.end() ;++ite )
     {
         int friendid = atoi( ite->c_str() );

         STRU_FRIEND_INFO friendRq;
         friendRq.m_userID = friendid;

         if( m_mapIDToUserInfo .find( friendid )!= m_mapIDToUserInfo.end() )
         {
            //好友在线发送登录人信息
             friendRq.m_state = 1;

             //可以从缓存里面取信息
             UserInfo* friender = m_mapIDToUserInfo[friendid];
             friendRq.m_iconID = friender->m_iconID;
             strcpy(friendRq.m_szName ,friender->m_userName) ;
             strcpy(friendRq.m_feeling , friender->m_feeling);

             //向好友发送登录人信息
             m_tcp->SendData(friender->m_fd , (char*)&loginRq , sizeof(loginRq)  );
         }else
         {
             //不在线从数据库里面取信息
             friendRq.m_state = 0;

             //从数据库里面取 更新info 再返回
             char sqlbuf[_DEF_SQLIEN ]="";
             sprintf(sqlbuf,"select name, icon , feeling from t_userInfo where id = %d",friendid);
             list<string> res;
             m_sql->SelectMysql( sqlbuf , 3, res);
             if( res.size() > 0 )
             {
                 strcpy(friendRq.m_szName, res.front().c_str());
                 res.pop_front();
                 friendRq.m_iconID = atoi(res.front().c_str());
                 res.pop_front();
                 strcpy(friendRq.m_feeling, res.front().c_str());
                 res.pop_front();
             }
         }

         //向登录的人发送好友的信息
         m_tcp->SendData( loginer->m_fd , (char*)& friendRq, sizeof(friendRq));
     }
}
//离线请求处理
void TcpKernel::OfflineRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d OfflineRq\n", clientfd);

    //解析包 , 给好友发离线rs
    STRU_OFFLINE_RQ * rq = (STRU_OFFLINE_RQ *)szbuf;

    char sqlbuf[ _DEF_SQLIEN ] = "";
    sprintf(sqlbuf, "select idB from t_friend where idA = '%d';", rq->m_userID);
    list<string> lst;
    m_sql->SelectMysql( sqlbuf, 1,lst);

    //删除id对应的信息
    auto ite = m_mapIDToUserInfo.find( rq->m_userID );
    if( ite != m_mapIDToUserInfo.end() )
    {//在房间里从房间移除该人  发送离开房间
        UserInfo* user = ite->second;

        STRU_LEAVEROOM_RS leaveRs;
        leaveRs.m_UserID = rq->m_userID;
        strcpy( leaveRs.szUserName , user->m_userName );
        if(user->m_roomID != 0 )
        {
            if( m_mapRoomIDToUserList.find(user->m_roomID) != m_mapRoomIDToUserList.end())
            {
                list<UserInfo*> lst = m_mapRoomIDToUserList[user->m_roomID];

                //遍历list
                auto iteList = lst.begin();
                while(iteList != lst.end())
                {
                    UserInfo * inner = *iteList;
                    //发给每一个用户 离开人的信息
                    if( inner->m_id != user->m_id )
                    {
                        this->SendMsgToOnlineClient(inner->m_id , (char*)&leaveRs,sizeof(leaveRs) );
                        ++iteList;
                    }else
                    {//从list移除这个节点
                        iteList = lst.erase(iteList);
                    }
                }
                //更新map
                m_mapRoomIDToUserList[ user->m_roomID ] = lst;
            }
        }
        m_mapIDToUserInfo.erase(ite);
    }
    while( lst.size() > 0)
    {
        int id = atoi( lst.front().c_str() );
        lst.pop_front();
        STRU_OFFLINE_RS rs;
        rs.m_userID = rq->m_userID;

        SendMsgToOnlineClient(  id , (char*)&rs , sizeof(rs) );
    }
}
