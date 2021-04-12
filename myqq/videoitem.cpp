#include "videoitem.h"
#include "ui_videoitem.h"
#include"QPainter"

VideoItem::VideoItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoItem)
{
    ui->setupUi(this);

    m_lastTime = QTime::currentTime();
    m_timer = new QTimer(this);
    connect(m_timer , SIGNAL(timeout()) , this , SLOT(slot_check()) );
    m_timer->start(1000);
}

VideoItem::~VideoItem()
{
    m_timer->stop();
    delete ui;
}

void VideoItem::setImage(const QImage &image)
{
    m_image = image;
    //更新时间
    m_lastTime = QTime::currentTime();
    this->repaint();//立即刷新绘图
}

void VideoItem::slot_check()
{
    if( m_lastTime.secsTo( QTime::currentTime()) > 3 ) //超时3秒 显示黑色
    {
        m_image.fill(Qt::black);
        //更新时间
        m_lastTime = QTime::currentTime();

        this->repaint();//立即刷新绘图
    }
}
//设置信息
void VideoItem::setInfo(const QString &name, int id)
{
    m_name = name;
    m_id = id ;
    ui->lb_name->setText(QString("用户名:%1").arg(m_name));
}
//鼠标点击 发送信号
void VideoItem::mousePressEvent(QMouseEvent *event)
{
    Q_EMIT SIG_itemClicked( m_id );
}


//绘图事件
void VideoItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //先画黑色背景
    painter.setBrush( Qt::black );
    painter.drawRect( 0,0, this->width() , this->height() );
    //等比例缩放图片  等比例变成控件这么大
    if( m_image.size().width()<= 0 ) return;

    QImage img = m_image.scaled( this->size() , Qt::KeepAspectRatio);
    //调整贴图位置
    int x =   this->width() - img.width() ;
    int y =  this->height() - ui->lb_name->height() - img.height();  //20 是标题的高

    x = x/2;
    y = y/2;
    //画图片
    painter.drawImage( QPoint(x,y) , img);

}
