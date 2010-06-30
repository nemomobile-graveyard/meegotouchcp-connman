#-*-Shell-Script-*-

load(meegotouch_defines)
include($$[QT_INSTALL_DATA]/mkspecs/dcpconfig.pri)
TEMPLATE = lib
PKGCONFIG += meegotouch
CONFIG += qt \
    meegotouch \
    duicontrolpanel \
    plugin \
    debug
LIBS += -L ../libconnman-qt -lconnman-qt
INCLUDEPATH += ../libconnman-qt
include(theme/theme.pri)

QT += dbus
TARGET = $$qtLibraryTarget(wifiapplet)
target.path = $$DCP_APPLET_DIR


STYLE_HEADERS += networkitemstyle.h

HEADERS += appletif.h \
    wifibrief.h \
    listwidget.h \
    networkitemstyle.h \
    networkitemview.h \
    networkitem.h \
    advancedpage.h \
    addnetworkpage.h

SOURCES += appletif.cpp \
           wifibrief.cpp \
           listwidget.cpp \
           networkitemview.cpp \
           networkitem.cpp \
           advancedpage.cpp \
           addnetworkpage.cpp 

INSTALLS += target
