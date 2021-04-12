#include "addfrienddlg.h"
#include "ui_addfrienddlg.h"

AddFriendDlg::AddFriendDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriendDlg)
{
    ui->setupUi(this);
}

AddFriendDlg::~AddFriendDlg()
{
    delete ui;
}

void AddFriendDlg::on_pb_addFriend_clicked()
{
    Q_EMIT SIG_AddFriend( ui->le_friendName->text() );
}


void AddFriendDlg::clear()
{
    ui->le_friendName->setText("");
}
