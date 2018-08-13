TEMPLATE = lib
CONFIG += plugin
TARGET = MyCore

INCLUDEPATH += ../../app

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../../Release/Lib
}
CONFIG(debug,debug|release){
    DESTDIR = ../../../Debug/Lib
}


LIBS += -lShell32

HEADERS += \
    mycore.h \
    ../../app/mycoreinterface.h

SOURCES += \
    mycore.cpp
