#ifndef SCREEN_READ_H
#define SCREEN_READ_H

#include <QObject>
#include<QTimer>
#include<QApplication>
#include<QDesktopWidget>
#include<QBuffer>
#include<QScreen>
#include<QImage>


class Screen_Read : public QObject
{
    Q_OBJECT
public:
    explicit Screen_Read(QObject *parent = nullptr);

signals:
    void SIG_sendScreenFrame( QImage& img);
public slots:
    void slot_getScreenFrame();
    void slot_openScreen();
    void slot_closeScreen();

private:
    QTimer * timer;
};

#endif // SCREEN_READ_H
