#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QWidget>
#include<QImage>
#include<QPaintEvent>
#include<QTime>
#include<QTimer>

namespace Ui {
class VideoItem;
}

class VideoItem : public QWidget
{
    Q_OBJECT
signals:
    void SIG_itemClicked(int id);
public:
    explicit VideoItem(QWidget *parent = nullptr);
    ~VideoItem();
public slots:
    void setImage(const QImage &image);

    void paintEvent(QPaintEvent *event); //repaint --> 立即执行该函数
    void slot_check();
    void setInfo(const QString &name , int id);

    void mousePressEvent(QMouseEvent *event);
private:
    Ui::VideoItem *ui;

    QImage m_image;

    QTime m_lastTime;
    QTimer* m_timer;
public:
    QString m_name;
    quint32 m_id;
};

#endif // VIDEOITEM_H
