TEMPLATE = lib
CONFIG += plugin
TARGET = SerialRS232Vir #仅仅是决定了生成dll文件的名字
QT += serialport widgets

INCLUDEPATH += ../SeriaRS232 \
                ../../app

include(../../app/my.pri)

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../../Release/Lib
}
CONFIG(debug,debug|release){
    DESTDIR = ../../../Debug/Lib
}


LIBS += -luser32

HEADERS += \
    serialrs232vir.h \
    serialrs232vir.h \
    ../SeriaRS232/serialrs232.h

SOURCES += \
    serialrs232vir.cpp \
    ../SeriaRS232/serialrs232.cpp


