#include "useritem.h"
#include "ui_useritem.h"

#include<QBitmap>
UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}

void UserItem::setInfo(int userID , QString name, int nSate , QString icon  , QString  feeling)
{
    m_userID = userID;
    m_name = name;
    m_state = nSate;
    m_icon = icon;
    m_feeling = feeling;

    ui->lb_name ->setText( m_name );
    ui->lb_feeling ->setText( feeling );

    if( nSate )
        ui->pb_icon ->setIcon( QIcon(icon) );
    else
    {
        QBitmap hui_tp;
        hui_tp.load( icon );
        ui->pb_icon ->setIcon( hui_tp );
    }

    this->repaint();//立即更新视图
}
//点击点击头像
void UserItem::on_pb_icon_clicked()
{
    Q_EMIT SIG_UserItemClicked();  // Q_EMIT
}
//鼠标双击响应
void UserItem::mouseDoubleClickEvent( QMouseEvent *event )
{
    emit SIG_UserItemClicked();
}
