#include "chatdialog.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent) :
    CustomMoveDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::setInfo(int id , QString name , QString icon )
{
    m_id = id;
    m_name = name;
    m_icon = icon;

    ui->lb_tittle->setText(QString("与%1的聊天").arg(name));
}
#include<QTime>
//发送聊天
void ChatDialog::on_pb_send_clicked()
{
    //ui->tb_chat 浏览
    //ui->te_chat 编辑
    QString content = ui->te_chat->toHtml();

    ui->tb_chat->append( QString("[ 我 ] %1").arg(QTime::currentTime().toString("hh:mm:ss")) ); //换行插入
    ui->tb_chat->append(content) ;
    ui->tb_chat->append(" ");

    ui->te_chat->clear();

    //发送信号
    Q_EMIT  SIG_SendChatContent( m_id , content);
}

Ui::ChatDialog *ChatDialog::getUi() const
{
    return ui;
}

//最小化
void ChatDialog::on_pb_min_clicked()
{
    this->showMinimized();
}
//最大化
void ChatDialog::on_pb_max_clicked()
{
    this->slot_showMax();
}

//关闭
void ChatDialog::on_pb_close_clicked()
{
    this->close();
}
