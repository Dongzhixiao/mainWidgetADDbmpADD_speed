#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T16:55:46
#
#-------------------------------------------------

QT       += core gui
QT += opengl
Qt += openglextensions

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mainWidget
TEMPLATE = app


SOURCES += main.cpp\
    logindlg.cpp \
    mainwindow.cpp \
    qands.cpp \
    generate_line.cpp \
    linescandialog.cpp \
    displaylayerdialog.cpp \
    bmpdialog.cpp \
    glwidget.cpp \
    skincoredialog.cpp \
    clipper.cpp

HEADERS  += \
    logindlg.h \
    mainwindow.h \
    generate_line.h \
    qands.h \
    linescandialog.h \
    displaylayerdialog.h \
    bmpdialog.h \
    glwidget.h \
    skincoredialog.h \
    clipper.hpp

FORMS    += \
    logindlg.ui \
    mainwindow.ui \
    linescandialog.ui \
    displaylayerdialog.ui \
    bmpdialog.ui \
    glwidget.ui \
    skincoredialog.ui

RESOURCES += \
    images.qrc

INCLUDEPATH += $$PWD

#LIBS += -L"D:\Qt\Qt5.5.0\5.5\msvc2013\lib" -lglut


#CONFIG += c++11 \c++0x
