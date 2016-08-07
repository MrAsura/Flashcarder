#-------------------------------------------------
#
# Project created by QtCreator 2015-05-16T14:19:15
#
#-------------------------------------------------

QT       += core gui qml quickwidgets
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flashcarder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dictedit.cpp \
    cardlist.cpp \
    cardfactory.cpp \
    cardedit.cpp

HEADERS  += mainwindow.h \
    dictedit.h \
    card.h \
    cardlist.h \
    cardfactory.h \
    cardedit.h

FORMS    += mainwindow.ui \
    dictedit.ui \
    cardedit.ui

RESOURCES += \
    qml.qrc


