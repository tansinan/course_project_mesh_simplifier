#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T00:40:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = MeshSimplifier
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    MSModel.cpp \
    MSVector2D.cpp \
    MSVector3D.cpp

HEADERS += \
    MSHeap.h \
    MSMatrix.h \
    MSModel.h \
    MSVector2D.h \
    MSVector3D.h
