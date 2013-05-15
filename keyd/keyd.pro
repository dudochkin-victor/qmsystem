# -------------------------------------------------
# Project created by QtCreator 2010-03-04T09:54:42
# -------------------------------------------------
QT += network
QT -= gui
TARGET = qmkeyd2
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    qmkeyd.cpp
HEADERS += qmkeyd.h

target.path = $$(DESTDIR)/usr/sbin
INSTALLS = target
