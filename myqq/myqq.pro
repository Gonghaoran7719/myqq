#-------------------------------------------------
#
# Project created by QtCreator 2020-11-03T18:22:45
#
#-------------------------------------------------

QT       += core gui

RC_ICONS = ./images/message.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


include(./netapi/netapi.pri)
INCLUDEPATH += $$PWD/netapi

include(./RecordVideo/RecordVideo.pri)
INCLUDEPATH += $$PWD/RecordVideo

include(./RecordAudio/RecordAudio.pri)
INCLUDEPATH += $$PWD/RecordAudio

include(./uiapi/uiapi.pri)
INCLUDEPATH += $$PWD/uiapi

TARGET = myqq
TEMPLATE = app


SOURCES += main.cpp\
        myqq.cpp \
    logindialog.cpp \
    chatdialog.cpp \
    roomdialog.cpp \
    useritem.cpp \
    IMToolBox.cpp \
    videoitem.cpp

HEADERS  += myqq.h \
    logindialog.h \
    chatdialog.h \
    roomdialog.h \
    useritem.h \
    IMToolBox.h \
    videoitem.h

FORMS    += myqq.ui \
    logindialog.ui \
    chatdialog.ui \
    roomdialog.ui \
    useritem.ui \
    videoitem.ui

RESOURCES += \
    resource.qrc
