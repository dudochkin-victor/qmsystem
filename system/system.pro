TEMPLATE = lib
TARGET = qmsystem2
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += SYSTEM_LIBRARY
QT = core network dbus

QMAKE_CXXFLAGS += -Wall -Wno-psabi

CONFIG += link_pkgconfig
PKGCONFIG += dsme dsme_dbus_if gconf-2.0 libiphb sensord timed

message("Compiling with mce support")
DEFINES += HAVE_MCE

linux-g++-maemo {
    message("Compiling with usb-moded-dev support")
    DEFINES += HAVE_USB_MODED_DEV
    PKGCONFIG += usb_moded

    message("Compiling with sysinfo support")
    DEFINES += HAVE_SYSINFO
    PKGCONFIG += sysinfo
} else {
    message("Compiling without usb-moded-dev support")
    message("Compiling without sysinfo support")
}

# DEFINES += HAVE_QMLOG
message("Compiling without qmlog support")

# Input
HEADERS += mainpage.h \
    msystemdbus_p.h \
    qmaccelerometer.h \
    qmaccelerometer_p.h \
    qmactivity.h \
    qmactivity_p.h \
    qmals.h \
    qmals_p.h \
    qmbattery.h \
    qmcabc.h \
    qmcallstate.h \
    qmcallstate_p.h \
    qmcompass.h \
    qmcompass_p.h \
    qmdevicemode.h \
    qmdevicemode_p.h \
    qmdisplaystate.h \
    qmdisplaystate_p.h \
    qmheartbeat.h \
    qmheartbeat_p.h \
    qmipcinterface_p.h \
    qmkeys.h \
    qmkeys_p.h \
    qmled.h \
    qmlocks.h \
    qmlocks_p.h \
    qmmagnetometer.h \
    qmmagnetometer_p.h \
    qmorientation.h \
    qmorientation_p.h \
    qmproximity.h \
    qmproximity_p.h \
    qmrotation.h \
    qmrotation_p.h \
    qmsensor.h \
    qmsensor_p.h \
    qmsysteminformation.h \
    qmsysteminformation_p.h \
    qmsystemstate.h \
    qmsystemstate_p.h \
    qmtap.h \
    qmtap_p.h \
    qmthermal.h \
    qmthermal_p.h \
    qmtime.h \
    qmusbmode.h \
    qmusbmode_p.h \
    qmwatchdog.h \
    qmwatchdog_p.h \
    system_global.h \
    qmtime_p.h
SOURCES += qmactivity.cpp \
    qmaccelerometer.cpp \ 
    qmals.cpp \
    qmcabc.cpp \
    qmcallstate.cpp \
    qmcompass.cpp \
    qmdevicemode.cpp \
    qmdisplaystate.cpp \
    qmheartbeat.cpp \
    qmipcinterface.cpp \
    qmkeys.cpp \
    qmled.cpp \
    qmlocks.cpp \
    qmorientation.cpp \
    qmsysteminformation.cpp \
    qmsystemstate.cpp \
    qmtap.cpp \
    qmthermal.cpp \
    qmproximity.cpp \
    qmtime.cpp \
    qmsensor.cpp \
    qmrotation.cpp \
    qmmagnetometer.cpp \
    qmwatchdog.cpp \
    qmusbmode.cpp

linux-g++-maemo {
    message("Compiling with bmeipc support")
    PKGCONFIG += bmeipc
    HEADERS += qmbattery_p.h
    SOURCES += qmbattery.cpp
} else {
    message("Compiling without bmeipc support")
    SOURCES += qmbattery_stub.cpp 
}

contextsubscriber { 
    DEFINES += PROVIDE_CONTEXT_INFO
    LIBS += -lcontextsubscriber
}
targetheaders.path = /usr/include/qmsystem2
targetheaders.files = $$HEADERS
target.path = /usr/lib/
INSTALLS += target \
    targetheaders
