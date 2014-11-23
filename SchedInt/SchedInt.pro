#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T22:37:07
#
#-------------------------------------------------

QT       +=core gui svg  widgets

TARGET = SchedInt
TEMPLATE = app

INCLUDEPATH +=../QRollout/

SOURCES += main.cpp\
        mainwindow.cpp \
    rollout_details.cpp

RESOURCES     = systray.qrc

HEADERS  += \
    rollout_details.h \
    mainwindow.h

FORMS    += mainwindow.ui \
    rollout_details.ui

LIBS = -L. -lQRollout
