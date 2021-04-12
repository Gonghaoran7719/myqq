#ifndef MYQQ_H
#define MYQQ_H

#include <QWidget>
#include<logindialog.h>
#include"qmytcpclient.h"
#include"IMToolBox.h"
#include<QMap>
#include"chatdialog.h"
#include"useritem.h"
#include<QCloseEvent>
#include"Packdef.h"
#include<QMenu>
#include"roomdialog.h"
#include<videoitem.h>
#include"IMToolBox.h"
#include"video_read.h"
#include"audio_read.h"
#include"audio_write.h"
#include"customwidget.h"

namespace Ui {
class MyQQ;
}

class MyQQ : public CustomMoveWidget
{
    Q_OBJECT

public:
    explicit MyQQ(QWidget *parent = 0);
    ~MyQQ();
    void setServerIP();

signals:
    void SIG_addFriend(QString name);
public slots:
    void slot_loginCommit(QString name,QString password);
    void slot_registerCommit(QString name,QString password);

    void slot_clientDealData(char* buf,int len);
    void slot_delete();
    void dealLoginRs(char* buf,int len);
    void dealRegisterRs(char* buf,int len);
    void dealForceOffline(char* buf,int len);
    void dealAddFriendRq(char* buf,int len);
    void dealAddFriendRs(char* buf,int len);
    void dealFriendInfo(char* buf,int len);

    void slot_SendChatContent(int id, QString content);
    void slot_dealMenu(QAction*action);
    void slot_AddFriendRq(QString name);
    void slot_quitRoom(int id);
private slots:
    void on_tb_friend_clicked();

    void on_tb_group_clicked();

    void on_tb_space_clicked();

    void slot_UserItemClicked();
    void on_pb_close_clicked();

    void closeEvent(QCloseEvent *event);
    void on_pb_menu_clicked();

    void dealChatRq(char *buf, int len);
    void dealChatRs(char *buf, int len);
    void slot_CreateRoom();
    void slot_JoinRoom(quint32 id);
    void dealCreateRoomRs(char *buf, int len);
    void dealJoinRoomRs(char *buf, int len);
    void dealMemberRq(char *buf, int len);
    void slot_openAudio();
    void slot_openVideo();
    void slot_closeAudio();
    void slot_closeVideo();
    void slot_sendVideoFrame(QImage &image);
    void dealLeaveRs(char *buf, int len);
    void dealVideoFrame(char *buf, int len);
    void slot_refreshImage(int id, QImage &image);
    void slot_videoItemClick(int id);
    void slot_sendAudioFrame(QByteArray ba);
    void dealAudioFrame(char *buf, int len);
    void on_pb_min_clicked();

    void dealOffLineRs(char *buf, int len);
private:
    Ui::MyQQ *ui;
    LoginDialog * m_login;
    QMyTcpClient *m_client;

    QMenu * m_MainMenu;
    int m_userID;

    IMToolItem * m_userList;
    char m_userName[MAX_SIZE];

    QMap<int , UserItem *> m_mapIDToUserItem;
    QMap<int , ChatDialog *> m_mapIDToChatDialog;
    QMap<int , VideoItem *> m_mapIDToVideoItem;
    QMap<int , Audio_Write*> m_mapIDToAudioWrite;


    IMToolItem *m_videoList;
    quint32 m_nRoomID;
    RoomDialog *m_Roomdlg;
    QString m_serverIP;

    Video_Read *m_pVideoRead;
    Audio_Read *m_pAudioRead;

//  AddFriendDlg * m_addFirend;
};

#endif // MYQQ_H
