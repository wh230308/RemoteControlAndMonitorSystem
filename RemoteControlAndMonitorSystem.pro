#-------------------------------------------------
#
# Project created by QtCreator 2018-11-28T14:59:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteControlAndMonitorSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mscmonitorform.cpp \
    cwpmonitorform.cpp \
    phonemonitorform.cpp \
    radiomonitorform.cpp \
    flowlayout.cpp \
    utility.cpp \
    udpclient.cpp \
    log.cpp \
    udpclientmanager.cpp \
    customcardlabel.cpp \
    customliulabel.cpp \
    customsvrlabel.cpp \
    customtooltip.cpp \
    customcwplabel.cpp \
    svraddrconfigdialog.cpp

HEADERS += \
        mainwindow.h \
    mscmonitorform.h \
    cwpmonitorform.h \
    phonemonitorform.h \
    radiomonitorform.h \
    flowlayout.h \
    utility.h \
    udpclient.h \
    log.hpp \
    udpclientmanager.h \
    customcardlabel.h \
    customliulabel.h \
    customsvrlabel.h \
    mscdatadeclaration.h \
    customtooltip.h \
    customcwplabel.h \
    svraddrconfigdialog.h

FORMS += \
        mainwindow.ui \
    svraddrconfigdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
