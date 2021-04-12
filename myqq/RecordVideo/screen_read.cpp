#include "screen_read.h"

Screen_Read::Screen_Read(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT( slot_getScreenFrame())   );
}


void Screen_Read::slot_getScreenFrame()
{
    QScreen *src = QApplication::primaryScreen();

//    QRect deskRect  =  QApplication::desktop()->screenGeometry();

    QPixmap map = src->grabWindow( QApplication::desktop()->winId()/* ,0,0 , deskRect.width() , deskRect.height()*/);

    QImage image = map.toImage();

 //   image = image.scaled( 1600, 900, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    emit SIG_sendScreenFrame(image);
}

void Screen_Read::slot_openScreen()
{
    timer->start(1000/8);
}
void Screen_Read::slot_closeScreen()
{
    timer->stop();
}
