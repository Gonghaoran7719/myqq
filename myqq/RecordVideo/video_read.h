#ifndef VIDEO_READ_H
#define VIDEO_READ_H


#include <QObject>
#include<QTimer>
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp"
#include<QImage>

using namespace cv;

#define FRAME_RATE (10)
class Video_Read : public QObject
{
    Q_OBJECT

public:
    explicit Video_Read(QObject *parent = 0);


signals:
    void SIG_sendVideoFrame( QImage& img);
public slots:
    void slot_getVideoFrame();
    void slot_openVideo();
    void slot_closeVideo();

private:

    VideoCapture cap;

    QTimer * timer;
};

#endif // VIDEO_READ_H
