QT += network core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ipc_demo
TEMPLATE = app

HEADERS       =  dialog.h \
    backendthread.h \
    parseconf.h \
    msgexchanger.h \
    protocol.h \
    frontendthreadreader.h \
    frontendthreadwriter.h \
    msg_ring.h

SOURCES       =  main.cpp dialog.cpp \
    backendthread.cpp \
    parseconf.cpp \
    msgexchanger.cpp \
    protocol.cpp \
    frontendthreadreader.cpp \
    frontendthreadwriter.cpp \
    msg_ring.cpp

FORMS += \
    dialog.ui

linux:LIBS += -llua -lm -ldl
win32:LIBS += -L"$$PWD/lua-5.2.3/lib" -llua52
win32:INCLUDEPATH += "lua-5.2.3/include"
