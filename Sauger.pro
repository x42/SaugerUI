#-------------------------------------------------
#
# Project created by QtCreator 2018-05-12T01:33:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sauger
TEMPLATE = app
ICON = Sauger.icns

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
  main.cc \
  mainwindow.cc \
  c-miio/aes.c \
  c-miio/json.c \
  c-miio/md5.c \
  c-miio/miio.c

HEADERS += \
  mainwindow.h \
  c-miio/aes.h \
  c-miio/json.h \
  c-miio/md5.h \
  c-miio/miio.h

FORMS += \
  mainwindow.ui
