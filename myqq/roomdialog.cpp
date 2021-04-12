#include "roomdialog.h"
#include "ui_roomdialog.h"
#include<QMessageBox>

RoomDialog::RoomDialog(QWidget *parent) :
    CustomMoveDialog(parent),
    ui(new Ui::RoomDialog),m_roomid(0)
{
    ui->setupUi(this);
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

//最小化
void RoomDialog::on_pb_min_clicked()
{
    this->showMinimized();
}

//退出房间
void RoomDialog::on_pb_quitRoom_clicked()
{
    if(  0 == m_roomid )
    {
        QMessageBox::about(this, "提示","先加入房间");
        return;
    }
    if( QMessageBox::question(this,"退出房间","确认退出房间?") == QMessageBox::No)
    {
        return;
    }
    //清空控件
    clear();

    //发信号
     Q_EMIT SIG_quitRoom(m_roomid);
}


//关闭
void RoomDialog::on_pb_close_clicked()
{
    on_pb_quitRoom_clicked();
}

//清空控件
void RoomDialog::clear()
{
    ui->pb_openAudio->setText("打开音频");
    ui->pb_openVideo->setText("打开视频");
    ui->lb_tittle->setText("房间号:---");
    m_roomid = 0;
}

//打开音频
void RoomDialog::on_pb_openAudio_clicked()
{
    if(  0 == m_roomid )
    {
        QMessageBox::about(this, "提示","先加入房间");
        return;
    }
    if( ui->pb_openAudio->text() == "打开音频" )
    {
        Q_EMIT SIG_openAudio();
        ui->pb_openAudio->setText("关闭音频");
    }else{
        Q_EMIT SIG_closeAudio();
        ui->pb_openAudio->setText("打开音频");
    }
}

//打开视频
void RoomDialog::on_pb_openVideo_clicked()
{
    if(  0 == m_roomid )
    {
        QMessageBox::about(this, "提示","先加入房间");
        return;
    }

    if( ui->pb_openVideo->text() == "打开视频" )
    {
        Q_EMIT SIG_openVideo();
        ui->pb_openVideo->setText("关闭视频");
    }else{
        Q_EMIT SIG_closeVideo();
        ui->pb_openVideo->setText("打开视频");
    }
}

Ui::RoomDialog *RoomDialog::getUi() const
{
    return ui;
}

//设置房间id
void RoomDialog::setRoomid(const quint32 &roomid)
{
    m_roomid = roomid;
    ui->lb_tittle->setText(QString("房间号:%1").arg(m_roomid));
}
//最大化
void RoomDialog::on_pb_max_clicked()
{
    this->slot_showMax();
}
