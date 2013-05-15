QT += dbus
QT -= gui
SOURCES += battery.cpp

LIBS += -lbmeipc
TARGET = battery-test
contextsubscriber {
    DEFINES += PROVIDE_CONTEXT_INFO
    LIBS += -lcontextsubscriber
}

include(../common-install.pri)
