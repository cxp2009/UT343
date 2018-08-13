#-------------------------------------------------
#
# Project created by QtCreator 2017-12-15T11:41:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += axcontainer
QT += printsupport
QT += network
QT += serialport
#QT += uiplugin
QT += xml

#CONFIG += plugin

TARGET = UT343
TEMPLATE = app

INCLUDEPATH += ../lib/CP2110 \
                ../lib/mycore

CONFIG(release,debug|release){
    DESTDIR = $$PWD/../../Release
}
CONFIG(debug,debug|release){
    DESTDIR = ../../Debug
}


QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

include(my.pri)

#win32{
#    INCLUDEPATH += "$$PWD\VISA\Include"
#    LIBS += "$$PWD\VISA\lib\msc\visa32.lib"
#}

LIBS += -luser32

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        widget.cpp \
    optiondialog.cpp \
    utils.cpp \
    dmmdata.cpp \
    datamodel.cpp \
    customwidget.cpp \
    dmmchart.cpp \
    histogram.cpp

HEADERS  += widget.h \
    optiondialog.h \
    utils.h \
    dmmdata.h \
    datamodel.h \
    mycoreinterface.h \
    serialdeviceinf.h \
    customwidget.h \
    serialrs232inf.h \
    serialrs232virinf.h \
    idmm.h \
    dmmchart.h \
    histogram.h


FORMS    += widget.ui

DISTFILES += \
    my.pri \
    prj.rc \
    ../../backup.bat \
    ../../cpnew.bat

#RC_FILE = UT8805.exe.rc

RC_FILE = prj.rc

RESOURCES += \
    res.qrc
