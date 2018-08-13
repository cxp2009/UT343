TEMPLATE = lib
CONFIG += plugin
TARGET = SerialRS232
QT += serialport

INCLUDEPATH += ../../app

include(../../app/my.pri)

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../../Release/Lib
}
CONFIG(debug,debug|release){
    DESTDIR = ../../../Debug/Lib
}


#LIBS += -lShell32

HEADERS += \
    serialrs232.h

SOURCES += \
    serialrs232.cpp


