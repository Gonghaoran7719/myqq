#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

signals:
    void SIG_registerCommit(QString name , QString password);
    void SIG_loginCommit(QString name , QString password);
private slots:
    void on_pb_register_clear_clicked();

    void slot_login_clear();
    void slot_register_clear();
    void on_pb_register_clicked();

    void on_pb_login_clicked();

private:
    Ui::LoginDialog *ui;


};

#endif // LOGINDIALOG_H
