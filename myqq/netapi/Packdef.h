
#ifndef __DEF_PACKDEF__
#define __DEF_PACKDEF__

#include<QHostAddress>
#include<QByteArray>
#include<QDebug>
#include<QFile>
#include<QDateTime>
#include <winsock2.h>
#define BOOL bool
#define DEF_PACK_BASE  (10000)


////注册
//#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
//#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)

////登录
//#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
//#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)

////创建房间
//#define DEF_PACK_CREATEROOM_RQ  (DEF_PACK_BASE + 4)
//#define DEF_PACK_CREATEROOM_RS  (DEF_PACK_BASE + 5)

////加入房间
//#define DEF_PACK_JOINROOM_RQ  (DEF_PACK_BASE + 6)
//#define DEF_PACK_JOINROOM_RS  (DEF_PACK_BASE + 7)

////房间列表请求
//#define DEF_PACK_ROOM_MEMBER    (DEF_PACK_BASE + 8)

////音频数据
//#define DEF_PACK_AUDIO_FRAME    (DEF_PACK_BASE + 9)

////退出房间请求
//#define DEF_PACK_LEAVEROOM_RQ   (DEF_PACK_BASE + 10)

////退出房间回复
//#define DEF_PACK_LEAVEROOM_RS   (DEF_PACK_BASE + 11)

////强制退出
//#define DEF_PACK_FORCE_OFFLINE   ( DEF_PACK_BASE + 20 )

typedef enum Net_PACK
{
    DEF_PACK_REGISTER_RQ = 10000,
    DEF_PACK_REGISTER_RS,

    DEF_PACK_LOGIN_RQ,
    DEF_PACK_LOGIN_RS,

    DEF_PACK_CREATEROOM_RQ,
    DEF_PACK_CREATEROOM_RS,

    DEF_PACK_JOINROOM_RQ ,
    DEF_PACK_JOINROOM_RS,

    DEF_PACK_ROOM_MEMBER ,

    DEF_PACK_AUDIO_FRAME,

    DEF_PACK_LEAVEROOM_RQ ,
    DEF_PACK_LEAVEROOM_RS ,

    DEF_PACK_ADD_FRIEND_RQ,
    DEF_PACK_ADD_FRIEND_RS,

    DEF_PACK_FRIEND_INFO,

    DEF_PACK_CHAT_RQ,
    DEF_PACK_CHAT_RS,

    DEF_PACK_OFFLINRE_RQ,
    DEF_PACK_OFFLINRE_RS,

    DEF_PACK_VIDEO_FRAME,
    DEF_PACK_FORCE_OFFLINE,
}Net_PACK;

//注册请求结果
#define userid_is_exist      0
#define register_sucess      1

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
#define user_online          3

//创建房间结果
#define room_is_exist        0
#define create_success       1

//加入房间结果
#define room_no_exist        0
#define join_success         1

//上传请求结果
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2
#define file_upload_refuse   3

//上传回复结果
#define fileblock_failed     0
#define fileblock_success    1

//下载请求结果
#define file_downrq_failed   0
#define file_downrq_success  1

//添加好友结果
#define no_this_user    0
#define user_refused    1
#define user_is_offline 2
#define add_success     3


#define _downloadfileblock_fail  0
#define _downloadfileblock_success	1

#define DEF_PACK_COUNT (100)

#define MAX_PATH            (280 )
#define MAX_SIZE            (60  )
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )
#define _DEF_PORT (8000)


/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096


typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户ID
    char     m_szPassword[MAX_SIZE];  //密码
}STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
    }
    PackType m_nType;   //包类型
    int  m_UserID;
    int  m_lResult ; //注册结果

}STRU_LOGIN_RS;


//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码

}STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果

}STRU_REGISTER_RS;

//创建房间请求
typedef struct STRU_CREATEROOM_RQ
{
    STRU_CREATEROOM_RQ()
    {
        m_nType = DEF_PACK_CREATEROOM_RQ;
        m_UserID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;

}STRU_CREATEROOM_RQ;

//创建房间回复
typedef struct STRU_CREATEROOM_RS
{
    STRU_CREATEROOM_RS()
    {
        m_nType= DEF_PACK_CREATEROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;

}STRU_CREATEROOM_RS;

//加入房间请求
typedef struct STRU_JOINROOM_RQ
{
    STRU_JOINROOM_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

}STRU_JOINROOM_RQ;

//加入房间回复
typedef struct STRU_JOINROOM_RS
{
    STRU_JOINROOM_RS()
    {
        m_nType= DEF_PACK_JOINROOM_RS;
        m_lResult = 0;
        m_RoomID = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int m_RoomID;
}STRU_JOINROOM_RS;

//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER;
        m_UserID =0;
        memset(m_szUser,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    char m_szUser[MAX_SIZE];

}STRU_ROOM_MEMBER_RQ;


//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEROOM_RQ;
        m_nUserId = 0;
        m_RoomId = 0;
    }
    PackType   m_nType;   //包类型
    int    m_nUserId; //用户ID
    int    m_RoomId;
}STRU_LEAVEROOM_RQ;

//离开房间回复
typedef struct STRU_LEAVEROOM_RS
{
    STRU_LEAVEROOM_RS()
    {
        m_nType = DEF_PACK_LEAVEROOM_RS;
        m_UserID = 0;
        memset(szUserName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szUserName[MAX_SIZE];

}STRU_LEAVEROOM_RS;

//添加好友请求
typedef struct STRU_ADD_FRIEND_RQ
{
    STRU_ADD_FRIEND_RQ()
    {
        m_nType = DEF_PACK_ADD_FRIEND_RQ;
        m_userID = 0;
        memset(m_szUserName,0,MAX_SIZE);
        memset(m_szAddFriendName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_userID;
    char m_szUserName[MAX_SIZE];
    char m_szAddFriendName[MAX_SIZE];

}STRU_ADD_FRIEND_RQ;

//添加好友回复
typedef struct STRU_ADD_FRIEND_RS
{
    STRU_ADD_FRIEND_RS()
    {
        m_nType = DEF_PACK_ADD_FRIEND_RS;
        m_userID = 0;
        m_friendID = 0;
        m_result = 0;
        memset(szAddFriendName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_userID;
    int m_friendID;
    int m_result;
    char szAddFriendName[MAX_SIZE];

}STRU_ADD_FRIEND_RS;

//好友信息
typedef struct STRU_FRIEND_INFO
{
    STRU_FRIEND_INFO()
    {
        m_nType = DEF_PACK_FRIEND_INFO;
        m_userID = 0;
        m_iconID = 0;
        m_state = 0;

        memset(m_szName,0,MAX_SIZE);
        memset(m_feeling,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_userID;
    int m_iconID;
    int m_state;
    char m_szName[MAX_SIZE];
    char m_feeling[MAX_SIZE];

}STRU_FRIEND_INFO;

//聊天请求
typedef struct STRU_CHAT_RQ
{
    STRU_CHAT_RQ()
    {
        m_nType = DEF_PACK_CHAT_RQ;
        m_userID = 0;
        m_friendID = 0;
        memset(m_ChatContent,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_userID;
    int m_friendID;
    char m_ChatContent[MAX_CONTENT_LEN];

}STRU_CHAT_RQ;

//聊天回复
typedef struct STRU_CHAT_RS
{
    STRU_CHAT_RS()
    {
        m_nType = DEF_PACK_CHAT_RS;
    }
    PackType   m_nType;   //包类型
    int m_userID;
    int m_friendID;
    int m_result;

}STRU_CHAT_RS;

//离线请求
typedef struct STRU_OFFLINE_RQ
{
    STRU_OFFLINE_RQ()
    {
        m_nType = DEF_PACK_OFFLINRE_RQ;
    }
    PackType   m_nType;   //包类型
    int m_userID;

}STRU_OFFLINE_RQ;

//离线回复
typedef struct STRU_OFFLINE_RS
{
    STRU_OFFLINE_RS()
    {
        m_nType = DEF_PACK_OFFLINRE_RS;
    }
    PackType   m_nType;   //包类型
    int m_userID;

}STRU_OFFLINE_RS;

typedef struct UserInfo
{
    UserInfo()
    {
         m_fd = 0;
         m_id = 0;
         m_iconID= 0;
         m_state= 0;

         memset(m_userName, 0 , MAX_SIZE);
         memset(m_feeling, 0 , MAX_SIZE);
    }
    int  m_fd;
    int  m_id;
    int  m_iconID;
    int  m_state;
    char m_userName[MAX_SIZE];
    char m_feeling[MAX_SIZE];

}UserInfo;

typedef struct STRU_FORCE_OFFLINE
{
    STRU_FORCE_OFFLINE()
    {
        m_nType = DEF_PACK_FORCE_OFFLINE;
        m_UserID = 0;
    }
    PackType   m_nType;   //包类型
    int m_UserID;

}STRU_FORCE_OFFLINE;

///音频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// QByteArray audioFrame;
///


///视频数据帧
/// 成员描述
/// int type;
/// int userId;
/// int roomId;
/// QByteArray videoFrame;
///





#endif
