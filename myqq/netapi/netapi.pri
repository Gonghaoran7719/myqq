QT       += network
LIBS += -lpthread libwsock32 libws2_32
LIBS += -lpthread libMswsock libMswsock

HEADERS += \
    $$PWD/qmytcpclient.h \
    $$PWD/TCPNet.h \
    $$PWD/Packdef.h


SOURCES += \
    $$PWD/qmytcpclient.cpp \
    $$PWD/TCPNet.cpp

