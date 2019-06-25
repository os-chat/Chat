TEMPLATE = app
TARGET = Chat
INCLUDEPATH += .

QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_LIBS += -lrt -lncurses -std=c++17

HEADERS += cmd_list.h common.h main_terminal.h receive_msg.h send_msg.h

SOURCES += main.cpp cmd_list.cpp common.cpp main_terminal.cpp receive_msg.cpp send_msg.cpp

