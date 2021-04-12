#include "myqq.h"
#include "ui_myqq.h"

#include<QMessageBox>
#include<QCryptographicHash>
#include<QInputDialog>
#include<QRegExp>
#include "ui_chatdialog.h"
#include<QSettings>
#include "ui_roomdialog.h"
#define MD5_KEY  (1234)

static QByteArray GetMD5(QString str)
{
    QString val = QString("%1_%2").arg(str).arg(MD5_KEY);
    QByteArray buf = val.toLocal8Bit(); //  toLatin1() ASCII 编码  toLocal8Bit Unicode编码
    QByteArray res =  QCryptographicHash::hash(buf ,  QCryptographicHash::Md5 );
    return res.toHex();  // ABF132...  32个长
}

MyQQ::MyQQ(QWidget *parent) :
    CustomMoveWidget(parent),
    ui(new Ui::MyQQ)
  ,m_userID(0),m_nRoomID(0),m_pAudioRead(NULL)
{
    ui->setupUi(this);

    m_login = new LoginDialog();

    connect( m_login ,SIGNAL(SIG_loginCommit(QString,QString)) ,
             this , SLOT(slot_loginCommit(QString,QString)) );
    connect( m_login ,SIGNAL(SIG_registerCommit(QString,QString)) ,
             this , SLOT(slot_registerCommit(QString,QString)) );

    m_login->show();

    m_serverIP = "192.168.43.75";
    this->setServerIP();
    m_client = new QMyTcpClient;
    m_client ->InitNetWork( ( char*)m_serverIP.toStdString().c_str() , _DEF_PORT);

    connect( m_client , SIGNAL(SIG_ReadyData(char*,int)) ,
             this , SLOT(slot_clientDealData(char*,int))  );

    m_userList = new  IMToolItem("我的好友");
    ui->wdg_box->addItem( m_userList );

//    UserItem * item =new UserItem;
//    m_userList->addItem( item );

//    item->setInfo( 1 , "colin" , 0 );
//    connect( item , SIGNAL(SIG_UserItemClicked()) , this , SLOT(slot_UserItemClicked()) );

//    ChatDialog * dialog = new ChatDialog;
//    dialog->setInfo( item->m_userID , item->m_name , item->m_icon);
//    connect( dialog , SIGNAL(SIG_SendChatContent(int,QString))
//             , this , SLOT(slot_SendChatContent(int,QString)) );
//    dialog->hide();

//    m_mapIDToUserItem[ item->m_userID ] = item;
//    m_mapIDToChatDialog[ dialog->m_id] = dialog;

//    m_addFirend = new AddFriendDlg;
//    m_addFirend->hide();

    connect(this , &MyQQ::SIG_addFriend , this , &MyQQ::slot_AddFriendRq  );
    m_MainMenu = new QMenu(this);
    m_MainMenu->addAction("添加好友");
    m_MainMenu->addSeparator();  //分隔符添加
    m_MainMenu->addAction("创建房间");
    m_MainMenu->addAction("加入房间");
    connect( m_MainMenu , SIGNAL(triggered(QAction*)) , this , SLOT(slot_dealMenu(QAction*)) );

    m_Roomdlg = new RoomDialog;
    connect( m_Roomdlg , SIGNAL(SIG_quitRoom(int)), this , SLOT(slot_quitRoom(int)) );
    connect( m_Roomdlg , SIGNAL(SIG_openAudio()), this ,  SLOT(slot_openAudio() ) );
    connect( m_Roomdlg , SIGNAL(SIG_openVideo()), this ,  SLOT(slot_openVideo() ) );
    connect( m_Roomdlg , SIGNAL(SIG_closeAudio()), this , SLOT(slot_closeAudio()) );
    connect( m_Roomdlg , SIGNAL(SIG_closeVideo()), this , SLOT(slot_closeVideo()) );

    m_videoList = new IMToolItem("房间成员",this);
    m_Roomdlg->getUi()->wdg_right->addItem(m_videoList);

    m_pVideoRead = new Video_Read;
    connect( m_pVideoRead , SIGNAL(SIG_sendVideoFrame(QImage&)) ,
             this, SLOT( slot_sendVideoFrame(QImage&) ) );

}

MyQQ::~MyQQ()
{
    delete ui;
}
//回收
void MyQQ::slot_delete()
{
    if( m_login )
    {
        delete m_login;
    }
    if( m_client )
    {
        delete m_client;
    }
    for( auto ite = m_mapIDToChatDialog.begin(); ite!=m_mapIDToChatDialog.end(); ++ite)
    {
        delete *ite;
    }
    m_mapIDToChatDialog.clear();

    if(m_Roomdlg)
    {
        delete m_Roomdlg;
    }
//    if(m_addFirend)
//    {
//        delete m_addFirend;
//    }

    if( m_pVideoRead )
    {
        m_pVideoRead->slot_closeVideo();
        delete m_pVideoRead;
    }
}

//登陆槽函数
void MyQQ::slot_loginCommit(QString name,QString password)
{
    //发送给服务器
    STRU_LOGIN_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
    strcpy_s( rq.m_szUser , buf);

    //转存用户名
    strcpy_s(m_userName , rq.m_szUser);
//    strTmp = password.toStdString();
//    buf = (char*) strTmp.c_str();
//    strcpy_s( rq.m_szPassword , buf);

    QByteArray ba = GetMD5(password);
    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
}

//注册槽函数
void MyQQ::slot_registerCommit(QString name,QString password)
{
    //发送给服务器
    STRU_REGISTER_RQ rq;
    std::string strTmp = name.toStdString();
    char* buf = (char*) strTmp.c_str();
    strcpy_s( rq.m_szUser , buf);

//    strTmp = password.toStdString();
//    buf = (char*) strTmp.c_str();
//    strcpy_s( rq.m_szPassword , buf);

    QByteArray ba = GetMD5(password);
    memcpy ( rq.m_szPassword , ba.data() , ba.length());

    m_client->SendData( (char*)&rq , sizeof(rq));
}

//客户端接收服务器信息处理
void MyQQ::slot_clientDealData(char* buf,int len)
{
    int nType = *(int*) buf; //*(int*) 按四个字节取
    switch( nType )
    {
        case DEF_PACK_LOGIN_RS:
            dealLoginRs(buf,len);
        break;
        case DEF_PACK_REGISTER_RS:
            dealRegisterRs(buf,len);
        break;
        case  DEF_PACK_FORCE_OFFLINE:
            dealForceOffline(buf,len);
        break;
        case DEF_PACK_ADD_FRIEND_RQ:
            dealAddFriendRq(buf,len);
        break;
        case DEF_PACK_ADD_FRIEND_RS:
            dealAddFriendRs(buf,len);
        break;
        case DEF_PACK_FRIEND_INFO:
            dealFriendInfo(buf,len);
        break;
        case DEF_PACK_CHAT_RQ:
            dealChatRq(buf,len);
        break;
        case DEF_PACK_CHAT_RS:
            dealChatRs(buf,len);
        break;
        case DEF_PACK_CREATEROOM_RS:
            dealCreateRoomRs(buf,len);
        break;
        case DEF_PACK_JOINROOM_RS:
            dealJoinRoomRs(buf,len);
        break;
        case DEF_PACK_ROOM_MEMBER:
            dealMemberRq(buf,len);
        break;
        case DEF_PACK_LEAVEROOM_RS:
            dealLeaveRs(buf,len);
        break;
        case DEF_PACK_OFFLINRE_RS:
            dealOffLineRs(buf,len);
        break;
        case DEF_PACK_VIDEO_FRAME:
            dealVideoFrame(buf,len);
        break;
        case DEF_PACK_AUDIO_FRAME:
            dealAudioFrame(buf,len);
        break;
    }
    delete[] buf;
}

////登录请求结果
//#define userid_no_exist      0
//#define password_error       1
//#define login_sucess         2
//#define user_online          3
//处理服务器登录回复
void MyQQ::dealLoginRs(char* buf,int len)
{
    STRU_LOGIN_RS * rs = (STRU_LOGIN_RS *)buf;
    switch( rs->m_lResult )
    {
    case userid_no_exist:
        QMessageBox::information( this->m_login , "提示", "用户不存在, 登录失败"  );
        break;
    case password_error:
        QMessageBox::information( this->m_login , "提示", "密码错误, 登录失败"  );
        break;
    case login_sucess:
        this->m_login->hide();
        this->show();
        this->ui->lb_name->setText( m_userName );
        m_userID = rs->m_UserID;
        break;
    }
}

////注册请求结果
//#define userid_is_exist      0
//#define register_sucess      1


//处理服务器注册回复
void MyQQ::dealRegisterRs(char* buf,int len)
{
    STRU_REGISTER_RS * rs = (STRU_REGISTER_RS *)buf;
    switch( rs->m_lResult )
    {
    case userid_is_exist:
        QMessageBox::information( this->m_login , "提示", "用户已存在, 注册失败"  );
        break;
    case register_sucess:
        QMessageBox::information( this->m_login , "提示", "注册成功"  );
        break;
    }
}
//处理强制下线
void MyQQ::dealForceOffline(char* buf,int len)
{
    STRU_FORCE_OFFLINE* off = (STRU_FORCE_OFFLINE*)buf;
    if( off->m_UserID == m_userID )
    {
        QMessageBox::information(this, "提示","异地登录,强制下线");
        this->close();
    }
}
//添加好友请求处理
void MyQQ::dealAddFriendRq(char* buf,int len)
{
    STRU_ADD_FRIEND_RQ * rq = (STRU_ADD_FRIEND_RQ *)buf;
    QString UserName = rq->m_szUserName;
    STRU_ADD_FRIEND_RS rs;
    rs.m_friendID = m_userID;
    rs.m_userID = rq->m_userID;

    if( QMessageBox::question( this, "添加好友" , QString("是否添加%1为好友").arg(UserName))
    == QMessageBox::Yes )
    {
        rs.m_result = add_success;
    }else
    {
        rs.m_result = user_refused;
    }
    m_client->SendData( (char*)&rs , sizeof(rs));
}
////添加好友结果
//#define no_this_user    0
//#define user_refused    1
//#define user_is_offline 2
//#define add_success     3
//添加好友回复处理
void MyQQ::dealAddFriendRs(char* buf,int len)
{
    STRU_ADD_FRIEND_RS* rs = (STRU_ADD_FRIEND_RS*)buf;
    switch( rs->m_result )
    {
    case user_refused:
        QMessageBox::about(this,"提示",QString("用户%1拒绝添加你为好友").arg(rs->szAddFriendName) );
        break;
    case add_success:
        QMessageBox::about(this,"提示",QString("添加%1为好友成功").arg(rs->szAddFriendName) );
        break;
    case user_is_offline:
        QMessageBox::about(this,"提示","用户不在线,添加不成功");
        break;
    case no_this_user:
        QMessageBox::about(this,"提示","查无此人,添加不成功");
        break;
    }
}

//处理聊天的请求
void MyQQ::dealChatRq(char* buf,int len)
{
    //解析
    STRU_CHAT_RQ *rq = (STRU_CHAT_RQ *)buf;
    //找到聊天窗口
    if( m_mapIDToChatDialog.find( rq->m_userID ) !=m_mapIDToChatDialog.end() )
    { //把信息粘到窗口
        ChatDialog *chat = m_mapIDToChatDialog[ rq->m_userID ];
        chat->getUi()->tb_chat->append( QString("[ %1 ] %2").arg(chat->m_name)
                                        .arg(QTime::currentTime().toString("hh:mm:ss")) );
        QString content = rq->m_ChatContent;
        chat->getUi()->tb_chat->append(content);
    }
}

//处理聊天的回复
void MyQQ::dealChatRs(char* buf,int len)
{
    qDebug()<<__func__;
    //解析
    STRU_CHAT_RS *rs = (STRU_CHAT_RS *)buf;
    if( rs->m_result == user_is_offline )//结果是不在线
    {//找到聊天窗口
        if( m_mapIDToChatDialog.find(rs->m_friendID) !=m_mapIDToChatDialog.end() )
        {//把离线信息粘到窗口
            ChatDialog *chat = m_mapIDToChatDialog[rs->m_friendID];
            chat->getUi()->tb_chat->append(QString("%1 用户离线, 消息未送达")
                                           .arg(QTime::currentTime().toString("hh:mm:ss")));
            chat->getUi()->tb_chat->append("");
            chat->show();
        }
    }
}

//处理创建房间回复
void MyQQ::dealCreateRoomRs(char* buf,int len)
{
    //解析
    STRU_CREATEROOM_RS * rs = (STRU_CREATEROOM_RS *)buf;
    if(rs->m_lResult )//结果成功
    {//设置房间标志
        m_nRoomID = rs->m_RoomId;
        //显示 视频创建
        if( m_mapIDToVideoItem.find(m_userID) == m_mapIDToVideoItem.end() )
        {//显示自己
            VideoItem *item = new VideoItem();
            item->setInfo(m_userName ,m_userID);
            connect( item, SIGNAL(SIG_itemClicked(int)) , this , SLOT(slot_videoItemClick(int)) );

            m_videoList->addItem(item); //显示到box
            m_mapIDToVideoItem[m_userID] = item;
        }

        //房间显示以及设置
        m_Roomdlg->setRoomid( rs->m_RoomId );
        m_Roomdlg->show();

        //创建音频采集
        m_pAudioRead = new Audio_Read;
        connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
                this , SLOT(slot_sendAudioFrame(QByteArray)) );
    }
}
//点击显示控件返还id --> 设置预览的控件的id
void MyQQ::slot_videoItemClick(int id)
{
    if( m_mapIDToVideoItem.find(id) != m_mapIDToVideoItem.end())
    {
        m_Roomdlg->getUi()->wdg_left->setInfo( m_mapIDToVideoItem[id]->m_name , id );
    }else
    {
        m_Roomdlg->getUi()->wdg_left->setInfo( "---" , id );
    }
}

//发送音频帧
void MyQQ::slot_sendAudioFrame(QByteArray ba)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;
    int nlen = ba.size() + 12 ;
    char *buf = new char[nlen];
    char * tmp = buf;

    *(int*)tmp = DEF_PACK_AUDIO_FRAME;
    tmp += sizeof(int);

    *(int*)tmp = m_userID;
    tmp += sizeof(int);

    *(int*)tmp =  m_nRoomID;
    tmp += sizeof(int);

    memcpy( tmp , ba.data() , ba.size());

    m_client->SendData( buf , nlen);
    delete[] buf;
}

//处理加入房间回复
void MyQQ::dealJoinRoomRs(char* buf,int len)
{
    //解析
    STRU_JOINROOM_RS * rs = (STRU_JOINROOM_RS *)buf;
    if( rs->m_lResult )//结果成功
    {//设置房间标志
        m_nRoomID = rs->m_RoomID;
        //显示 视频创建
        if( m_mapIDToVideoItem.find(m_userID) == m_mapIDToVideoItem.end() )
        {//显示自己
            VideoItem *item = new VideoItem();
            item->setInfo(m_userName ,m_userID);
            connect( item, SIGNAL(SIG_itemClicked(int)) , this , SLOT(slot_videoItemClick(int)) );

            m_videoList->addItem(item); //显示到box
            m_mapIDToVideoItem[m_userID] = item;
        }

        //房间显示以及设置
        m_Roomdlg->setRoomid( rs->m_RoomID );
        m_Roomdlg->show();

        //创建音频采集
        m_pAudioRead = new Audio_Read;
        connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
                this , SLOT(slot_sendAudioFrame(QByteArray)) );
    }
}

//处理成员请求
void MyQQ::dealMemberRq(char* buf,int len)
{
    //解析包
    STRU_ROOM_MEMBER_RQ * rq = (STRU_ROOM_MEMBER_RQ *)buf;
    //创建对应的显示模块
    if( m_mapIDToVideoItem.find(rq->m_UserID) == m_mapIDToVideoItem.end() )
    {//显示
        VideoItem *item = new VideoItem();
        item->setInfo(rq->m_szUser ,rq->m_UserID);
        connect( item, SIGNAL(SIG_itemClicked(int)) , this , SLOT(slot_videoItemClick(int)) );

        m_videoList->addItem(item); //显示到box
        m_mapIDToVideoItem[rq->m_UserID] = item;
    }

    //创建对应的声音模块
    if( m_mapIDToAudioWrite.find(rq->m_UserID ) == m_mapIDToAudioWrite.end() )
    {
        Audio_Write * audioWrite = new Audio_Write;
        m_mapIDToAudioWrite[ rq->m_UserID ] = audioWrite;
    }
}

//处理视频帧
void MyQQ::dealVideoFrame(char* buf,int len)
{

    ///视频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray videoFrame;

    char* tmp = buf;
    tmp += sizeof(int);

    int userId =  *(int*) tmp; //按照四个字节取数据
    tmp += sizeof(int);

    int roomId = *(int*) tmp;
    tmp += sizeof(int);

    // tmp 指向图片的缓冲区 需要把他变为图片
    QByteArray bt( tmp , len - 12 );
    QImage img ;
    img.loadFromData( bt );

    this->slot_refreshImage( userId ,  img );
}

//处理音频帧
void MyQQ::dealAudioFrame(char* buf,int len)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;

    char* tmp = buf;
    tmp += sizeof(int);

    int userId =  *(int*) tmp; //按照四个字节取数据
    tmp += sizeof(int);

    int roomId = *(int*) tmp;
    tmp += sizeof(int);

    if( m_mapIDToAudioWrite.find( userId) != m_mapIDToAudioWrite.end() )
    {
        QByteArray bt(tmp , len -12);
        Audio_Write * pAudioWrite = m_mapIDToAudioWrite[userId];
        pAudioWrite->slot_net_rx( bt );
    }
}

//离开房间回复
void MyQQ::dealLeaveRs(char* buf,int len)
{
    //解析包
    STRU_LEAVEROOM_RS* rs = (STRU_LEAVEROOM_RS* )buf;
    //清除视频模块
    auto ite = m_mapIDToVideoItem.find(rs->m_UserID);
    if( ite != m_mapIDToVideoItem.end() )
    {
        VideoItem *item = m_mapIDToVideoItem[rs->m_UserID];
        m_videoList->removeItem( item);//界面去销毁
        delete item;
        m_mapIDToVideoItem.erase(ite);
    }
    //清除音频模块
    auto iteAudio = m_mapIDToAudioWrite.find(rs->m_UserID);
    if( iteAudio != m_mapIDToAudioWrite.end() )
    {
        Audio_Write *item = m_mapIDToAudioWrite[rs->m_UserID];
        delete item;
        m_mapIDToAudioWrite.erase(iteAudio);
    }
}

//离线回复
void MyQQ::dealOffLineRs(char* buf,int len)
{
    STRU_OFFLINE_RS* rs = (STRU_OFFLINE_RS*) buf;

    {
        auto ite = m_mapIDToUserItem.find( rs->m_userID );
        if( ite != m_mapIDToUserItem.end() )
        {
             UserItem * item = *ite;
             qDebug()<< __FUNCTION__;
             item->setInfo( item->m_userID , item->m_name , 0, item->m_icon , item->m_feeling );
        }
    }
}

//从配置文件读取ip地址
void MyQQ::setServerIP( )
{
 //   1、创建对象
    QString arg1 = "./config.ini"; //这个是你ini文件的路径
    QSettings setting(arg1,QSettings::IniFormat,NULL); // 打开ini文件

 //   3、读取内容
    setting.beginGroup("net");
    QVariant res = setting.value("ip");
//    这样就会读取到key对应的value值。
    QString ip = res.toString();
    qDebug()<< ip;
    if( !ip.isEmpty() ) m_serverIP = ip;
}

//好友信息更新
void MyQQ::dealFriendInfo(char* buf,int len)
{
    STRU_FRIEND_INFO *info = (STRU_FRIEND_INFO *)buf;

    //查找map
    if( m_mapIDToUserItem.find( info->m_userID ) != m_mapIDToUserItem.end() )
    { //有 , 更新
         UserItem* item =  m_mapIDToUserItem[ info->m_userID];

         item->setInfo(info->m_userID,info->m_szName,info->m_state/*,QString(":/tx/%1.png").arg(info->m_iconID) ,info->m_feeling*/);

         if( m_mapIDToChatDialog.find( info->m_userID ) == m_mapIDToChatDialog.end() )
         {
             ChatDialog *chat = m_mapIDToChatDialog[info->m_userID] ;
             chat->setInfo( info->m_userID,info->m_szName,QString(":/tx/%1.png").arg(info->m_iconID));
         }

    }else
    {//没有 添加到控件 并且创建聊天窗口
         UserItem* item = new UserItem;
         item->setInfo( info->m_userID,info->m_szName,info->m_state/*,QString(":/tx/%1.png").arg(info->m_iconID) ,info->m_feeling */);
         connect( item , SIGNAL(SIG_UserItemClicked()) , this ,SLOT( slot_UserItemClicked()) );
         m_mapIDToUserItem[ info->m_userID]  = item;
         ChatDialog *chat =    new  ChatDialog;
         chat->setInfo( info->m_userID,info->m_szName,QString(":/tx/%1.png").arg(info->m_iconID));

         connect( chat , SIGNAL(SIG_SendChatContent(int,QString))
                  , this , SLOT(slot_SendChatContent(int,QString)) );

         m_mapIDToChatDialog[ info->m_userID ] = chat;

         m_userList->addItem(item);
    }
}

//点击好友
void MyQQ::on_tb_friend_clicked()
{
    ui->tb_friend->setChecked(true);
    ui->tb_group->setChecked(false);
    ui->tb_space->setChecked(false);

    ui->stk_page->setCurrentIndex(0);
}
//点击群组
void MyQQ::on_tb_group_clicked()
{
    ui->tb_friend->setChecked(false);
    ui->tb_group->setChecked(true);
    ui->tb_space->setChecked(false);

    ui->stk_page->setCurrentIndex(1);
}
//点击空间
void MyQQ::on_tb_space_clicked()
{
    ui->tb_friend->setChecked(false);
    ui->tb_group->setChecked(false);
    ui->tb_space->setChecked(true);

    ui->stk_page->setCurrentIndex(2);
}
//点击用户响应
void MyQQ::slot_UserItemClicked()
{
    UserItem * item = (UserItem *) sender();  //获取被点击的用户对象

    //QMessageBox::about( this , "提示" , QString("用户%1被点击").arg(item->m_name));
    //查 map 然后弹出 聊天对话
    if( m_mapIDToChatDialog.find( item->m_userID ) != m_mapIDToChatDialog.end())
    {
        //找到了
       ChatDialog * dlg =  m_mapIDToChatDialog[ item->m_userID ];
       dlg->show();
    }else
    {
        ChatDialog * dialog = new ChatDialog;
        dialog->setInfo( item->m_userID , item->m_name , item->m_icon);
        connect( dialog , SIGNAL(SIG_SendChatContent(int,QString))
                 , this , SLOT(slot_SendChatContent(int,QString)) );
        dialog->show();
        m_mapIDToChatDialog[ dialog->m_id] = dialog;
    }
}

//发送信息
void MyQQ::slot_SendChatContent(int id ,QString content)
{
    STRU_CHAT_RQ rq;
    rq.m_userID = m_userID;
    rq.m_friendID = id;
    std::string  str = content.toStdString();
    const char* buf = str.c_str();
    strcpy( rq.m_ChatContent , buf );

    m_client->SendData( (char*)&rq , sizeof(rq));
}

//菜单处理
void MyQQ::slot_dealMenu(QAction* action)
{
    if( action->text() == "添加好友" )
    {
//        m_addFirend->clear();
//        m_addFirend->show();
        QString name = QInputDialog::getText(this,"添加好友","添加好友名");
        QRegExp reg("[A-Z0-9a-z]{1,10}");
        if(reg.exactMatch(name) )
            Q_EMIT SIG_addFriend(name);
        else
            QMessageBox::about(this, "提示","用户名非法");

    }else if( action->text() == "创建房间" )
    {
        if( m_nRoomID == 0 )
        {
            slot_CreateRoom();
        }else
        {
            QMessageBox::about(this, "提示","已经在房间内");
        }
    }else if( action->text() == "加入房间" )
    {
        if( m_nRoomID != 0 )
        {
            QMessageBox::about(this, "提示","已经在房间内");
        }
        QString id = QInputDialog::getText(this,"加入房间","输入加入房间号");
        QRegExp reg("[0-9]{1,6}");
        if(reg.exactMatch(id) )
             slot_JoinRoom( id.toInt());
        else
            QMessageBox::about(this, "提示","房间号非法");
    }
}

//发送创建房间的请求
void MyQQ::slot_CreateRoom()
{
    STRU_CREATEROOM_RQ rq;
    rq.m_UserID = m_userID;

    m_client->SendData((char*)&rq , sizeof(rq));
}
//发送加入房间的请求
void MyQQ::slot_JoinRoom( quint32 id)
{
    STRU_JOINROOM_RQ rq;
    rq.m_RoomID = id;
    rq.m_UserID = m_userID;

    m_client->SendData((char*)&rq , sizeof(rq));
}


//添加好友请求
void MyQQ::slot_AddFriendRq(QString name)
{
    //异常
    //1.不可以添加自己
    QString UserName = m_userName;
    if( name  == UserName )
    {
        QMessageBox::about(this,"提示", "不可以添加自己");
        return;
    }
    //2.已经好友 不可添加
    for( auto ite = m_mapIDToUserItem.begin() ; ite != m_mapIDToUserItem.end() ; ++ite)
    {
        UserItem * item = *ite ;
        if( item ->m_name == name )
        {
            QMessageBox::about(this,"提示", "已经是好友");
            return;
        }
    }
    //发送添加请求
    STRU_ADD_FRIEND_RQ rq;
    rq.m_userID = m_userID;
    strcpy_s( rq.m_szUserName , m_userName);
    std::string tmp = name.toStdString();
    const char * buf = tmp.c_str();
    strcpy_s( rq.m_szAddFriendName,  buf);

    m_client->SendData( (char*)&rq,sizeof(rq));
}
//离开房间
void MyQQ::slot_quitRoom(int id)
{
    //写离开房间包
    STRU_LEAVEROOM_RQ rq;
    rq.m_nUserId = m_userID;
    rq.m_RoomId = m_nRoomID;

    m_client->SendData((char*)&rq, sizeof(rq));

    //暂停视频采集
    this->slot_closeVideo();
    //暂停音频采集
    this->slot_closeAudio();
    //销毁
    // 销毁显示模块
    for( auto ite = m_mapIDToVideoItem.begin() ; ite != m_mapIDToVideoItem.end();)
    {
        VideoItem * item = *ite;
        m_videoList->removeItem( item ); //界面上清除

        delete item;
        //清空map
        ite = m_mapIDToVideoItem.erase(ite);
    }

    //销毁声音播放
    for( auto ite = m_mapIDToAudioWrite.begin() ; ite != m_mapIDToAudioWrite.end();)
    {
        delete *ite ;
        //清空map
        ite = m_mapIDToAudioWrite.erase(ite);
    }

    //房间标志清空
    m_nRoomID = 0;
    //房间隐藏
    m_Roomdlg->hide();
}
//打开音频
void MyQQ::slot_openAudio()
{
    if(m_pAudioRead)
    {
        m_pAudioRead->ResumeAudio();
    }
}
//打开视频
void MyQQ::slot_openVideo()
{
    if( m_pVideoRead )
    {
        m_pVideoRead->slot_openVideo();
    }
}
//关闭音频
void MyQQ::slot_closeAudio()
{
    if(m_pAudioRead)
    {
        m_pAudioRead->PauseAudio();
    }
}
//关闭视频
void MyQQ::slot_closeVideo()
{
    if( m_pVideoRead )
    {
        m_pVideoRead->slot_closeVideo();
    }
}

#include<QBuffer>
//视频采集的图片发送出去
void MyQQ::slot_sendVideoFrame(QImage& image)
{
    //显示到当前控件上
    slot_refreshImage( m_userID , image );

    //将图片编码成 jpg 格式
    QByteArray bt;
    QBuffer buf(&bt);  // QBuffer 与QByteArray 去关联

    image.save( &buf , "JPEG"); //以jpeg格式存储图片到bt

    //发送视频帧到服务器
    ///视频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray videoFrame;
    int nlen = 12+ bt.size();
    char *szbuf = new char[ nlen ];
    char * tmp = szbuf;
    *(int*) tmp = DEF_PACK_VIDEO_FRAME;//按照四个字节向空间里面写
    tmp += sizeof(int);
    *(int*) tmp = m_userID ;
    tmp += sizeof(int);
    *(int*) tmp = m_nRoomID ;
    tmp += sizeof(int);

    memcpy( tmp , bt.data() , bt.size() );
    m_client->SendData( szbuf , nlen);

    delete[] szbuf;
}

//视频采集的图片更新控件
void MyQQ::slot_refreshImage( int id , QImage& image )
{
    //首先找到控件 设置图片
    if( m_mapIDToVideoItem.find(id) != m_mapIDToVideoItem.end() )
    {
        VideoItem *item = m_mapIDToVideoItem[id];
        item->setImage( image );
        //判断预览是否是当前的id , 如果是也设置图片

        if( m_Roomdlg->getUi()->wdg_left->m_id == id  )
        {
            m_Roomdlg->getUi()->wdg_left->setImage(image);
        }
    }
}

//关闭
void MyQQ::on_pb_close_clicked()
{
    if( QMessageBox::question( this , "提示","是否退出应用?") == QMessageBox::Yes )
    {
        this->close();
    }
}

//关闭事件回收
void MyQQ::closeEvent(QCloseEvent *event)
{
    //发送离线消息
    STRU_OFFLINE_RQ rq;
    rq.m_userID = m_userID;
    m_client->SendData( (char*)&rq , sizeof(rq));

    this->slot_delete();

    Sleep(500);
    event->accept();
}

//点击弹出菜单
void MyQQ::on_pb_menu_clicked()
{
    QPoint p = QCursor::pos();
    int y = p.y()- m_MainMenu->sizeHint().height();
    m_MainMenu->exec( QPoint(p.x() , y) );
}
//最小化
void MyQQ::on_pb_min_clicked()
{
    this->showMinimized();
}
