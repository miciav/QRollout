#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T22:37:07
#
#-------------------------------------------------

QT       +=core gui svg  widgets

TARGET = SchedInt
TEMPLATE = app
CONFIG-=app_bundle

INCLUDEPATH +=../QRollout/

SOURCES += main.cpp\
        mainwindow.cpp \
    rollout_details.cpp

RESOURCES = systray.qrc

HEADERS  += \
    rollout_details.h \
    mainwindow.h

FORMS    += mainwindow.ui \
         rollout_details.ui

LIBS = -L. -lQRollout


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QRollout/release/ -lQRollout
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QRollout/debug/ -lQRollout
else:unix: LIBS += -L$$OUT_PWD/../QRollout/ -lQRollout

INCLUDEPATH += $$PWD/../QRollout
DEPENDPATH += $$PWD/../QRollout
