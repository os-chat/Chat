TEMPLATE = app
TARGET = Chat
INCLUDEPATH += .

QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_LIBS += -lrt -lncurses

HEADERS += window.h cmd_list.h common.h main_interface.h main_terminal.h receive_msg.h send_msg.h main_console.h

SOURCES += main.cpp window.cpp cmd_list.cpp common.cpp main_interface.cpp main_terminal.cpp receive_msg.cpp send_msg.cpp main_console.cpp

