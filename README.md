# 基本介绍
基于OpenCV+FFmpeg+SDL完成的实时会议的通信软件，采用QT编程进行客户端的UI的设计，全工程使用了IMtoolBOX的抽屉类，和UserItem类实现了好友列表和视屏窗口；
服务器使用Threadpoll+epoll模型实现数据交互、业务处理， 在1核2GB内存的Linux，最大连接数可达几十万

我的CSDN ： https://blog.csdn.net/GHR7719


# 编译环境
myqq 运行环境为Qt5.9.9   
server ubuntu任意qt环境即可

# OpenCV文件说明
项目所需的库
注：打开工程后，需配置相应的库的路径



会议聊天室
背景
疫情来袭，兴起了线上教学、工作的模式，展现了“互联网+”时代的优势；群众们万众一心、足不出户抗疫的同时，工作和学习却有条不紊的进行，直播、会议类的APP起了关键的作用。
那么能否开发一款类似的程序呢？
为将所学知识融为一起并加以应用和练习，并迎互联网的热潮，故开发此会议聊天室。


目录
背景	1
一、	设计目的	2
1.1功能与要求	2
1.2 环境选择	2
二、	实现	3
2.1网络模块	3
2.2客户端	3
2.3 会议模块	4
2.4 服务器模块	12
三、	优化	12
3.1视频与音频	12
3.2心跳机制	12
3.2传输效率	12
四、	总结	13
4.1 遇到的问题	13
4.2 学习经历	13




一、	设计目的
1.1功能与要求
1. 实现用户注册、登录
2. 修改密码、个人资料
3. 好友之间信息发送、文件传输
4. 添加删除好友，强制下线
5. 创建、加入多人实时会议

实现要求: 
会议时延迟不得超于1秒,直播不限制,文件传输应有秒传,断点续传功能,支持大文件传输等
密码不能明文传输,保证账户的安全.
服务器应承受一定的并发量,并高可用,容灾等
客户端应简单明了,操作方便

1.2 环境选择
客户端的实现使用的是Windows操作系统下的QT, Qt是一个多平台的C++图形用户界面应用程序框架。
服务器使用的是ubuntu16.04,64位版本,同样安装qt作为开发环境;
数据库使用的是Linux MySQL5.7


安装MySQL5.7
1. sudo apt-get install mysql-server
2. sudo apt-get install mysql-client 
3. sudo apt-get install libmysqlclient-dev →develop 开发库

sudo netstat -tap | grep mysql 
通过上述命令检查之后，如果看到有 mysql 的 socket 处于 listen 状态则表示安装成功。 tcp 0 0 localhost:mysql  *:*  LISTEN 9764/mysqld
登陆 mysql 数据库可以通过如下命令：
mysql -u root -p
show databases; 就可以查看当前的数据库
我们选择 mysql 数据库就行下一步操作，使用 use mysql 命令，显示当前数据库的表单： show tables ;
二、	实现
2.1网络模块
为了实现可靠性数据传输,项目采用TCP协议
编写函数
  bool InitNetWork(char* szBufIP , unsigned short port); 
  int ConnectToServer( char* szBufIP);
int SendData(char* szbuf,int nLen);
void UnInitNetWork();
自定义协议
自定义协议可解决TCP的粘包问题,增加了程序的扩展性,可读性等
部分协议展示
typedef enum Net_PACK
{
    DEF_PACK_REGISTER_RQ = 10000,
    DEF_PACK_REGISTER_RS,

    DEF_PACK_LOGIN_RQ,
    DEF_PACK_LOGIN_RS,

    DEF_PACK_CREATEROOM_RQ,
    DEF_PACK_CREATEROOM_RS,

    DEF_PACK_JOINROOM_RQ ,
DEF_PACK_JOINROOM_RS,
} Net_PACK;
2.2客户端
Qt导入网络模块
编写了网络模块的文件后,对其进行封装加载到项目
include(./netapi/netapi.pri) 
INCLUDEPATH += $$PWD/netapi

客户端导入自定义协议 导入 Packetdef.h 编写登录, 注册对应的协议头, 以及对应的请求和回复的结构体. 提交登录和注册请求
关于登录, 处理登录请求, 先根据用户名查表取密码, 如果没有, 那么没有该用户, 有的话, 比对密码 是否一致, 不一致, 返回密码错误, 一致返回登录成功. 除此之外, 这里还可以加是否在线的检测. 此外 登录还需要有, 获取好友列表, 以及告诉好友上线, 以及发送离线期间的所有请求.

登录注册
在登录窗口发送信号:
SIG_loginCommit(QString name , QString password); 
SIG_registerCommit(QString name , QString passord);
登录加密的方法
1.	MD5 以及 MD5 加盐 2.对称加密 AES 和非对称加密 3.仿照 ssl 握手 4.验证信息以及上次登录 ip , 绑定 ip , 绑定 mac 5.手机验证登录 6.微信验证登录 7.做人脸识别

强制下线请求 
当用户的登录时, 发现映射表中已经存在该用户了, 那么, 强制原来的用户下线, 然后录入新用户信息, 并返回结果.

数据库设计:
创建新数据库 myqq. 服务器的数据库设置为 myqq. 
添加数据表 t_user 
create table t_user ( id bigint unsigned AUTO_INCREMENT primary key, name varchar (260) , password varchar(260) ); 
添加测试数据 
insert into t_user (name,password) values ( 'test','test'); 
好友表 t_friend 
create table t_friend ( idA bigint unsigned , idB bigint unsigned); 

用户信息表 t_userInfo : 用户 id , 用户名, 图标号, 心情 → 注册用户时, 写入表 名字 图标号, 默认0 , 心情”
create table t_userInfo ( id bigint unsigned , name varchar (260) , icon int , feeling varchar(260));

好友列表的获取:
发送好友信息,若客户端1和客户端2是好友,1登录时发送个人信息给服务器,服务器转发给客户端2,在将客户端2的信息返还给客户端1;


添加好友:
服务器转发客户端A的请求给客户端B 若B在线则转发,若不在线则记录,等上线时进行转发, 返回结果是否同意给客户端A,同意之后将好友列表更新,写入数据库.

聊天的请求与回复
通过服务器转发聊天请求,如果不在线存入数据库中,等上线时在进行转发

2.3 会议模块
创建房间 
客户端发送创建房间请求 ,服务器随机生成房间号返回

加入房间
客户端发送加入房间请求,服务器查看房间是否存在,若存在将客户端1添加至房间列表
给房间的每一个客户端发送成员列表

退出房间
客户端1回收房间资源,服务器收到某客户端1退出房间的请求后,查找房间,得到用户列表,向每一个客户端发送客户端1退出房间--每一个客户端接受后删除客户端1的资源
房间用户列表
//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER;
        m_UserID =0;
        memset(m_szUser,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    char m_szUser[MAX_SIZE];

}STRU_ROOM_MEMBER_RQ;

延迟分析:
视频会议的特点: 低延迟, 音频, 视频 , 多人聊天. 关于低延迟, 视频会议与直播的项目是不同的. 直 播项目是一个 1 对多的过程, 并且, 是允许 5 秒左右的网络延迟的, 而视频会议, 一般延迟要求 1 秒以内


视频
对于音频视频方面, 项目中是需要采集摄像头, 采集声音, 以及 利用白板功能采集桌面;
视屏处理流程如下:
摄像头采集  编码  网络通道  解码  图像显示
对于视频方面的考虑, 首先获取到视频数据后, 是比较大的, 网络传输很占用带宽, 所以需要编码(压 缩), 来减小数据大小. 那么要使用什么编码合适呢? 可以使用 ffmpeg 进行编码. 视频方面可 以使用它将视频编码为 h264 格式数据再进行传输.

高级功能: 
（1）动态调整视频的清晰度 在 Internet 上，网络速度是实时动态变化的，所以，在视频会议中，为了优先保证语音的通话质 量，需要实时调整视频的相关参数，其最主要的就是调整编码的清晰度，因为清晰度越高，对带宽要 求越高，反之亦然。 比如，当检测网络繁忙时，就自动降低编码的清晰度，以降低对带宽的占用。 
（2）自动丢弃视频帧 同样网络繁忙时，还有一个方法，就是发送方是主动丢弃要发送的视频帧，这样在接收方看来， 就是帧频 fps 降低了

音频
对于音频方面, 和上面的描述类似, 也是需要编码后再传输, 而音频方面的编码, 可选的方式有很多, 可以使用 ffmpeg 或者 sdl, 这两种一般优先选择 ffmpeg 更好. 而音频的处理还没有这么简单. 在编码 之前, 采集音频数据, 一般还需要考虑一些问题: 回音消除, 降噪, 抑制声音断断续续, 静音检测, 混音.
音频的主要流程
语音采集  编码  网络通道  解码  语音播放
语音采集指的是从麦克风采集音频数据，即声音样本转换成数字信号。其涉及到几个重要的参数：采样频率、采样位数、声道数。
假设我们将采集到的音频帧不经过编码，而直接发送，那么我们可以计算其所需要的带宽要求， 仍以上例：320*100 =32KBytes/s，如果换算为 bits/s，则为 256kb/s。这是个很大的带宽占用。而通 过网络流量监控工具，我们可以发现采用类似 QQ 等 IM 软件进行语音通话时，流量为 3-5KB/s，这 比原始流量小了一个数量级。而这主要得益于音频编码技术。所以，在实际的语音通话应用中，编码 这个环节是不可缺少的。目前有很多常用的语音编码技术，像 G.729、G.711、iLBC、AAC、SPEEX 等等。 
当一个音频帧完成编码后，即可通过网络发送给通话的对方。对于语音对话这样 Realtime 应用， 低延迟和平稳是非常重要的，这就要求我们的网络传送非常顺畅。 
当对方接收到编码帧后，会对其进行解码，以恢复成为可供声卡直接播放的数据。 
完成解码后，即可将得到的音频帧提交给声卡进行播放。

高级功能介绍: 
如果仅仅依靠上述的技术就能实现一个效果良好的应用于广域网上的语音对话系统，那就太 easy 了。正是由于很多现实的因素为上述的概念模型引入了众多挑战，使得网络语音系统的实现不是那么 简单，其涉及到很多专业技术。一个“效果良好”的语音对话系统应该达到如下几点：低延迟，背景 噪音小，声音流畅、没有卡、停顿的感觉，没有回音。 
对于低延迟，只有在低延迟的情况下，才能让通话的双方有很强的 Realtime 的感觉。当然，这个主要取决于网络的速度和通话双方的物理位置的距离，就单纯软件的角度，优化的可能性很小。 
（1）回音消除 
现在大家几乎都已经都习惯了在语音聊天时，直接用 PC 或笔记本的声音外放功能。当使用外放 功能时，扬声器播放的声音会被麦克风再次采集，传回给对方，这样对方就听到了自己的回音。 回音消除的原理简单地来说就是，回音消除模块依据刚播放的音频帧，在采集的音频帧中做一些类似抵消的运算，从而将回声从采集帧中清除掉。这个过程是相当复杂的，因为它还与你聊天时所 处的房间的大小、以及你在房间中的位置有关，因为这些信息决定了声波反射的时长。 智能的回音 消除模块，能动态调整内部参数，以最佳适应当前的环境。现在做的比较好的回音消除有, webrtc. webrtc 顾名思义是引用在网页端的技术, 是谷歌最早提出的算法. 
（2）噪声抑制 
噪声抑制又称为降噪处理，是根据语音数据的特点，将属于背景噪音的部分识别出来，并从音频帧中过滤掉。有很多编码器都内置了该功能。 
（3）抖动缓冲区 
抖动缓冲区（JitterBuffer）用于解决网络抖动的问题。所谓网络抖动，就是网络延迟一会大一会小， 在这种情况下，即使发送方是定时发送数据包的（比如每 100ms 发送一个包），而接收方的接收就无 法同样定时了，有时一个周期内一个包都接收不到，有时一个周期内接收到好几个包。如此，导致接 收方听到的声音就是一卡一卡的。 JitterBuffer 工作于解码器之后，语音播放之前的环节。即语音解码完成后，将解码帧放入 JitterBuffer，声卡的播放回调到来时，从 JitterBuffer 中取出最老的一帧进行播放。 JitterBuffer 的缓冲深度取决于网络抖动的程度，网络抖动越大，缓冲深度越大，播放音频的延迟 就越大。所以，JitterBuffer 是利用了较高的延迟来换取声音的流畅播放的，因为相比声音一卡一卡来 说，稍大一点的延迟但更流畅的效果，其主观体验要更好。 当然，JitterBuffer 的缓冲深度不是一直不变的，而是根据网络抖动程度的变化而动态调整的。当 网络恢复到非常平稳通畅时，缓冲深度会非常小，这样因为 JitterBuffer 而增加的播放延迟就可以忽略 不计了。 
（4）静音检测 
在语音对话中，要是当一方没有说话时，就不会产生流量就好了。静音检测就是用于这个目的的。 静音检测通常也集成在编码模块中。静音检测算法结合前面的噪声抑制算法，可以识别出当前是否有 语音输入，如果没有语音输入，就可以编码输出一个特殊的的编码帧（比如长度为 0）。特别是在多人视频会议中，通常只有一个人在发言，这种情况下，利用静音检测技术而节省带宽还是非常可观的。 
（5）混音 
在视频会议中，多人同时发言时，我们需要同时播放来自于多个人的语音数据，而声卡播放的缓 冲区只有一个，所以，需要将多路语音混合成一路，这就是混音算法要做的事情。

采集视频--采集摄像头
采集视频采用 opencv , 版本 4.10, 其实可以使用 qt 自带的 QCamera , 不过, opencv 便于未来扩展, 比如视频采集, 加入美颜, 所以采用 opencv 来做摄像头的采集.

加载导入OpenCV的库(根据库所在的路径)
INCLUDEPATH+=D:\QQQQ\OpenCV\include\opencv\
                   D:\QQQQ\OpenCV\include\opencv2\
                    D:\QQQQ\OpenCV\include

LIBS+= D:\QQQQ\OpenCV\lib\libopencv_calib3d2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_contrib2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_core2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_features2d2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_flann2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_gpu2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_highgui2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_imgproc2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_legacy2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_ml2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_objdetect2410.dll.a\
       D:\QQQQ\OpenCV\lib\libopencv_video2410.dll.a

然后需要在 exe 同级的路径下, 添加 dll 文件
得到 QImage 图像
Mat frame;
cap>>frame;
cvtColor(frame,frame,CV_BGR2RGB);

QImage image((unsigned const char*)frame.data,frame.cols,frame.rows,QImage::Format_RGB888);
    image = image.scaled( 320,240, Qt::KeepAspectRatio,Qt::FastTransformation );
 Q_EMIT SIG_sendVideoFrame(image);
采集图片
使用定时器定时采集图片发送图片
timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT( slot_getVideoFrame())   );
打开视频
  cap.open(0);//打开默认摄像头
    if(!cap.isOpened()){
        QMessageBox::information(NULL,tr("提示"),tr("视频没有打开"));
        return;
    }
timer->start(1000/FRAME_RATE);// FRAME_RATE 帧率, 此时可以定义为10
关闭
	timer->stop();
    if(cap.isOpened())
        cap.release();



采集图片之后,将图像压缩为JPEG,通过TCP协议发送给服务器
void slot_sendVideoFrame(QImage& image)
{
    //显示到当前控件上
    slot_refreshImage( m_userID , image );
    //将图片编码成 jpg 格式
    QByteArray bt;
    QBuffer buf(&bt);  // QBuffer 与QByteArray 去关联
    image.save( &buf , "JPEG"); //以jpeg格式存储图片到bt
    //发送视频帧到服务器
    ///视频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray videoFrame;
    int nlen = 12+ bt.size();
    char *szbuf = new char[ nlen ];
    char * tmp = szbuf;
    *(int*) tmp = DEF_PACK_VIDEO_FRAME;//按照四个字节向空间里面写
    tmp += sizeof(int);
    *(int*) tmp = m_userID ;
    tmp += sizeof(int);
    *(int*) tmp = m_nRoomID ;
    tmp += sizeof(int);
    memcpy( tmp , bt.data() , bt.size() );
    m_client->SendData( szbuf , nlen);
    delete[] szbuf;
}
服务器
服务器接受数据然后转发给房间内其他成员. 
客户端
其他客户端接受服务器转发的数据,然后加载图片,显示到控件

采集视频--采集桌面
采集桌面和采集摄像头类似, 只是获取图片的方式不同
	void Screen_Read::slot_getScreenFrame()
	{
   	 QScreen *src = QApplication::primaryScreen();
	QPixmap map = src->grabWindow( QApplication::desktop()->winId()/* ,0,0 , deskRect.width() , deskRect.height()*/);

     QImage image = map.toImage();
 	  emit SIG_sendScreenFrame(image);
}

采集音频
采集音频, 参数是 采样率 8kHz, 单声道, 采样位数 16, 按照该格式采集的数据大小是 640 个字节一帧声音. 
数据传输率 128kbps.
 语音信号的带宽为 300-3400HZ,采样频率不少于 8KHZ, 
若量化精度为 16 位,单声道输出,那么每秒钟的数据量和每小时的数据量各是多少? 
数据传输率=8khz*16 b*1=128 kbps 
每 秒 钟 的 数 据 量 =128*1000*1/8B=16000B=16000kB/1024=15.625KB ( 也 就 是 每 秒 128/8 = 16000b/s(每秒 16k 字节)) 
1 秒钟 16000 字节, 一帧 640 个字节, 那么 1 秒音频是 25 帧. 
获取声音和播放声音的组件是 Qt 自带的 QAudioInput 和 QAudioOutput , 
(使用时需要在 pro 文件中添加媒体库的引用 Qt += multimedia
1.	首先初始化设备
QAudioFormat format;
format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    //format.setByteOrder(QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    //format.setSampleType(QAudioFormat::SignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
      // qWarning()<<"default format not supported try to use nearest";
       QMessageBox::information(NULL , "提示", "打开音频设备失败");
       format = info.nearestFormat(format);
    }
----------------------------------------------
当接收到声卡的数据时. 我们可以采用定时器, 每隔一段时间从缓存中,取音频数据
if(audio_in)
  {
            delete audio_in;
            audio_in = new QAudioInput(format, this);
            myBuffer_in = audio_in->start();
    }
    
    m_playState = state_play;

    timer = new QTimer(this);
connect( timer , &QTimer::timeout , 
this , &Audio_Read::readMore );
    timer->start(1000/40);

声音采集暂停
audio_in->stop();
声音采集恢复, 可以将原 QAudioInput 对象回收, 重新再申请 
if(audio_in) { 
delete audio_in; 
audio_in = new QAudioInput(format, this); 
myBuffer_in = audio_in->start(); 
} 
timer = new QTimer(this); 
connect( timer , &QTimer::timeout , this , 
&Audio_Read::readMore ); 
timer->start(1000/40);

自定义槽函数 readMore()
处理声音, 要做的工作是, 
1).对声音进行静音检测处理,消除没有说话时沙沙的噪声
2).降噪处理 
3).编码音频(可以采用 webrtc , speex , G.711, G.722) 
4).发送数据.

发送数据
使用自定义协议,通过TCP发送
//发送音频帧
void slot_sendAudioFrame(QByteArray ba)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;
    int nlen = ba.size() + 12 ;
    char *buf = new char[nlen];
    char * tmp = buf;
    *(int*)tmp = DEF_PACK_AUDIO_FRAME;
    tmp += sizeof(int);
    *(int*)tmp = m_userID;
    tmp += sizeof(int);
    *(int*)tmp =  m_nRoomID;
    tmp += sizeof(int);
    memcpy( tmp , ba.data() , ba.size());
    m_client->SendData( buf , nlen);
    delete[] buf;
}

服务器接收, 并转发给房间内其他成员. 
每一个成员对应一个 QAudioOutput, 用来播放声音. 

接收数据, 解码得到声音并播放

2.4 服务器模块
目前选择采用epoll+线程池的模型


三、	优化
3.1视频与音频
同步与延迟
当前使用视频采用获取图片压缩 jpeg 发送的形式, 并没有音画同步, 因为本身延时很低 音频编解码采用的是 speex , 可以让压缩比为 88:640, 效果较好, 静音检测 VAD , 以及降噪 NS 采用 webrtc, 编解码其实也可以使用 webrtc 来做, webrtc 采用的编解码为 G.711, 简单方便, 不过这里没有使用.

音视频相互的影响 
音频和视频的传输采用同一条 tcp, 如果想降低因为图片较大, 影响语音的传输, 可以采用额外一条 tcp 专门传视频

3.2心跳机制
用户非正常退出, 不会发离线包, 可能会使服务器中状态异常. 可以采用心跳机制. 
具体实现思路: 
让一个线程, 循环每隔一段时间向所有在线人发送心跳请求(也可客户端主动请求) , 服务器用户信息结构中, 多一个计数, 记录没收到心跳倒计时. 比如心跳倒计时初始为 5 , 开启一个线程, 每隔一段时间(比如 5 秒)对每个用户的心跳倒计时-1, 心跳倒计时为 0, 说明离线, 给每个用户发该用户离线信息, 将该用户信息从服务器移除.而定时器发心跳请求是 5s 每次, 那么每隔 5s , 服务器接收到客户端的心跳, 每次收到心跳, 就重置该用户的心跳倒计时(即置 5). 简单来说就是给用户 25s 的时间来发心跳 , 期间每隔 5 秒会询问一次, 如果一直服务器没有收到心跳, 判定离线.

3.2传输效率
多条TCP连接
对于 qq 聊天功能, 监听的 tcp 可以是两个, 一个用来收信息, 一个用来传文件. 互不影响.
文件传输
可以新开辟工作者线程进行传输,也可以使用多条TCP连接将文件分快传输
断点续传分为暂停续传和退出续传. 
暂停的续传, 就是指, 接收方, 可以采取暂停的方式, 暂时停止传输, 等合适时间, 继续传输. 可以采 用发送端发送文件块, 接收端接收文件块返回确认包的方式传输, 这样就可以暂停. 但是为了让传输 速度最优, 可以采用阶段式的回复确认( 也就是比如每次从文件中读 4k 发送, 每发 100 个包返回一次确认, 包里面包含比如 1-100 , 2-100 的标志, 等到服务器接收到 100-100 即 100 个包 , 返回确认), 这样就可以实现暂停. 续传的时候, 询问接收端文件的位置, 接收端通过查看文件当前存储的字节数, 返回发送者要开始续传的位置. 
退出续传, 在用户退出, 或用户程序崩溃时, 再次进入程序, 可以实现的续传.实现办法, 在本地存储 正在进行的任务(文件名及路径 , 这个正在进行的任务, 在每次完成任务删除), 每次程序开启时加载 正在进行的任务到界面, 用户可以点击继续传输, 发请求询问从什么位置开始, 并继续任务.

四、	总结
此项目, 登录注册时采用MD5算法对密码进行密文传输，登录后显示好友列表，聊天记录保存到用户本地，采用了TCP协议进行客户端与服务器的连接，使用了C/S架构，并使用中介者模式进行类的管理；服务器模块封装了MySQL的API用来处理频繁的请求，
服务器采用了完成端口和线程池的模型解决并发问题 会议模块使用了OpenCV进行摄像头的开启,关闭,图片采集等，使用FFmpeg实现了音频和视频的编解码。
4.1 遇到的问题 
历史三个月,此项目终于可已告一段落了
回顾项目的开发中,遇见了很多问题,提出了很多解决方案,每当能够顺利的debug时都会无比的兴奋,起初密码的加密问题,还是采用了最容易实现的MD5,虽然并不是真正的加密,但也避免了明文传输的危害,后面的学习生活中,还要更注重于信息安全问题.
在延迟要做到1s以下的要求时,用牺牲视屏清晰度和流畅度来降低延迟,通过OpenCV和定时器改变采集图片的时间,使用JPEG的格式传输图片,极大的缩小了文件的体积,实现低延迟会议的要求. 在后面的学习中还要考虑高清视屏的传输问题以及音视频同步,音频降噪等问题.
后续需考虑:
使用C/S架构,使用高并发且高可用的服务器模型,系统容灾问题,数据备份等
采用MySQL的数据库,编写API,用循环队列处理数据库的读写,如何优化,可否使用用内存数据库,曾加访问速度以及可靠性
4.2 学习经历
从刚开始的环境搭建,了解QT的开发,信号与槽的使用,到安装了解Linux下MySQL的使用,和局域网,广域网的测试,并部署到云服务器上,上线运行,IP配置等,
了解了云服务器之后,搭建了自己的个人网站,绑定了域名,将项目打包成app上传到网站上提供下载, 并学习web网页制作用来宣传自己,以及介绍自己的项目等
互联网是一个新潮的产业,它的开源特性使得我们更加轻易的学习各种前沿的知识,通过各大国内外网站的知识获取,我们能从中非常顺利的学习
本项目中很多思路代码都来源于网络,在此,向提供资料和源代码的前人致谢 !
