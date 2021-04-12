#include "video_read.h"
#include<QDebug>
#include<QMessageBox>
#include<QImage>
#include<QImageReader>
#include<QBuffer>

Video_Read::Video_Read(QObject  *parent)
    : QObject(parent)
{
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT( slot_getVideoFrame())   );
}

void Video_Read::slot_openVideo()
{
    cap.open(0);//打开默认摄像头
    if(!cap.isOpened()){
        QMessageBox::information(NULL,tr("提示"),tr("视频没有打开"));
        return;
    }
    timer->start(1000/FRAME_RATE);
}
void Video_Read::slot_closeVideo()
{
    timer->stop();
    if(cap.isOpened())
        cap.release();
}

void Video_Read::slot_getVideoFrame()
{
    Mat frame;
    cap>>frame;
    cvtColor(frame,frame,CV_BGR2RGB);

    QImage image((unsigned const char*)frame.data,frame.cols,frame.rows,QImage::Format_RGB888);
    image = image.scaled( 320,240, Qt::KeepAspectRatio,Qt::FastTransformation );
    Q_EMIT SIG_sendVideoFrame(image);

}

