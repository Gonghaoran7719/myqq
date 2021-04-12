#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

public:
    explicit UserItem(QWidget *parent = 0);
    ~UserItem();


    void setInfo(int userID , QString name, int nSate ,
                 QString icon =QString(":/tx/0.png") , QString m_feeling= QString("比较懒,什么也没写") );
signals:
    void SIG_UserItemClicked();
public slots:
    void mouseDoubleClickEvent( QMouseEvent *event );

    void on_pb_icon_clicked();

private:
    Ui::UserItem *ui;
public:
    int m_userID;
    QString m_name;
    QString m_icon;
    QString m_feeling;
    int m_state;
};

#endif // USERITEM_H
