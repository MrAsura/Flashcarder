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
    cardedit.cpp \
    card.cpp \
    cardlisteditor.cpp \
    cardviewer.cpp \
    global.cpp

HEADERS  += mainwindow.h \
    dictedit.h \
    card.h \
    cardlist.h \
    cardfactory.h \
    cardedit.h \
    global.h \
    cardlisteditor.h \
    cardviewer.h

FORMS    += mainwindow.ui \
    dictedit.ui \
    cardedit.ui \
    cardlisteditor.ui \
    cardviewer.ui

RESOURCES += qml.qrc

DISTFILES += data/types/CardTypeOne.qml \
    data/types/SimpleCardTypeOne.qml
