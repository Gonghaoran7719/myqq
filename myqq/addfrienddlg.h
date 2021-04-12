#ifndef ADDFRIENDDLG_H
#define ADDFRIENDDLG_H

#include <QWidget>

namespace Ui {
class AddFriendDlg;
}

class AddFriendDlg : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendDlg(QWidget *parent = 0);
    ~AddFriendDlg();
signals:
    void SIG_AddFriend(QString name);
private slots:
    void on_pb_addFriend_clicked();
public:
    void clear();
private:
    Ui::AddFriendDlg *ui;
};

#endif // ADDFRIENDDLG_H
