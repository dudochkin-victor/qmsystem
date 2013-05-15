QT += testlib
QT -= gui
TEMPLATE = app
QMAKE_CXXFLAGS += -Wall -Wno-psabi

INCLUDEPATH += ../../system ../../../system
#LIBS += -L../../system -L../../../system -lqmsystem2
LIBS += -lqmsystem2
QMAKE_LIBDIR_FLAGS += -L../../system -L../../../system 
target.path = /usr/bin
INSTALLS += target
