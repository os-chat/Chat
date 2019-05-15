TEMPLATE = app
TARGET = Chat
INCLUDEPATH += .

QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_LIBS += -lrt

HEADERS += window.h
SOURCES += main.cpp window.cpp
