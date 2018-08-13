TEMPLATE = lib
CONFIG += plugin
TARGET = CustomDmm
QT += serialport

INCLUDEPATH += ../../app

include(../../app/my.pri)

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../../Release/Lib
}
CONFIG(debug,debug|release){
    DESTDIR = ../../../Debug/Lib
}

HEADERS += \
    dmm.h \
    mydmm.h \
    dmmdata.h \
    dmmdata.h

SOURCES += \
    dmm.cpp \
    mydmm.cpp \
    dmmdata.cpp \
    dmmdata.cpp


