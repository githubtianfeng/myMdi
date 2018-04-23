#-------------------------------------------------
#
# Project created by QtCreator 2018-03-18T11:19:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myMdi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mdichild.cpp

HEADERS  += mainwindow.h \
    mdichild.h

FORMS    += mainwindow.ui

RESOURCES += \
    myImage.qrc
RC_ICONS = app.ico
RC_FILE = myapp.rc
