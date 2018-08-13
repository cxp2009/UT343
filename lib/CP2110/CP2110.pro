TEMPLATE = lib
CONFIG += plugin
CONFIG += no_batch
TARGET = CP2110



QT += widgets

LIBS += -luser32

INCLUDEPATH += ../../app

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../../Release
}
CONFIG(debug,debug|release){
    DESTDIR = ../../../Debug
}


HEADERS += \
    cp2110.h \
    ../../app/serialdev.h \
    ../../app/serialdeviceinf.h \
    ../../app/cp2110inf.h

SOURCES += \
    cp2110.cpp \
    ../../app/serialdev.cpp

