#-------------------------------------------------
#
# Project created by QtCreator 2013-03-26T05:43:30
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ss13telemap
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    calibrationdialog.cpp

HEADERS  += mainwindow.h \
    calibrationdialog.h

FORMS    += mainwindow.ui \
    calibrationdialog.ui
