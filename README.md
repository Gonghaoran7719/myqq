# 基本介绍
基于OpenCV+FFmpeg+SDL完成的实时会议的通信软件，采用QT编程进行客户端的UI的设计，全工程使用了IMtoolBOX的抽屉类，和UserItem类实现了好友列表和视屏窗口；
服务器使用Threadpoll+epoll模型实现数据交互、业务处理， 在1核2GB内存的图形界面的Ubuntu中，最大连接数可达四万以上


# 编译环境
myqq 运行环境为Qt5.9.9   
server ubuntu任意qt环境即可
