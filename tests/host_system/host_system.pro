TARGET = host-system-test
SOURCES += host_system.cpp

QT += dbus
QT -=gui
TEMPLATE = app


INCLUDEPATH += ../../system ../../../system

#LIBS += -L../../system -L../../../system -lqmsystem2
QMAKE_LIBDIR_FLAGS += -L../../system/ -L../../../system -lqmsystem2

target.path = /usr/bin
INSTALLS += target


