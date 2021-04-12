#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include"customwidget.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public CustomMoveDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = 0);
    ~ChatDialog();


    void setInfo(int id , QString name , QString icon = QString(":/tx/0.png"));
signals:
    void SIG_SendChatContent(int id, QString content);
private slots:
    void on_pb_send_clicked();

    void on_pb_min_clicked();

    void on_pb_max_clicked();

    void on_pb_close_clicked();

private:
    Ui::ChatDialog *ui;
public:
    int m_id;
    QString m_name;
    QString m_icon;

    Ui::ChatDialog *getUi() const;
};

#endif // CHATDIALOG_H
