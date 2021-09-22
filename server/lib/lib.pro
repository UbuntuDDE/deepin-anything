TEMPLATE = lib
TARGET = deepin-anything-server-lib
QT += dbus concurrent
QT -= gui
CONFIG += link_pkgconfig
PKGCONFIG += udisks2-qt5 mount

include(../common.pri)

SOURCES += \
    dasplugin.cpp \
    dasfactory.cpp \
    dasinterface.cpp \
    daspluginloader.cpp \
    lftmanager.cpp \
    lftdisktool.cpp

HEADERS += \
    dasdefine.h \
    dasplugin.h \
    dasfactory.h \
    dasinterface.h \
    daspluginloader.h \
    lftmanager.h \
    lftdisktool.h

INCLUDEPATH += ../../library/inc

CONFIG(debug, debug|release) {
    LIBS += -L$$_PRO_FILE_PWD_/../../library/bin/debug -lanything
    DEPENDPATH += $$_PRO_FILE_PWD_/../../library/bin/debug
    unix:QMAKE_RPATHDIR += $$_PRO_FILE_PWD_/../../library/bin/debug
} else {
    LIBS += -L$$_PRO_FILE_PWD_/../../library/bin/release -lanything
}

isEmpty(LIB_INSTALL_DIR) {
    LIB_INSTALL_DIR = $$[QT_INSTALL_LIBS]
}

DEFINES += QMAKE_VERSION=\\\"$$VERSION\\\"

PLUGINDIR = $$LIB_INSTALL_DIR/$${TARGET}/plugins

readme.files += README.txt
readme.path = $$PLUGINDIR/handlers

CONFIG(debug, release|debug) {
    PLUGINDIR = $$_PRO_FILE_PWD_/../plugins:$$PLUGINDIR
}

DEFINES += PLUGINDIR=\\\"$$PLUGINDIR\\\"

target.path = $$LIB_INSTALL_DIR

isEmpty(PREFIX): PREFIX = /usr

includes.files += \
    dasdefine.h \
    dasfactory.h \
    dasplugin.h \
    dasinterface.h \
    lftmanager.h

includes.path = $$PREFIX/include/deepin-anything-server

INSTALLS += target includes readme

CONFIG += create_pc create_prl no_install_prl

QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = $$TARGET
QMAKE_PKGCONFIG_DESCRIPTION = Deepin anything server library
QMAKE_PKGCONFIG_INCDIR = $$includes.path
