#-*-Shell-Script-*-

load(meegotouch_defines)
TEMPLATE = lib
VERSION = 0.0.3
PKGCONFIG += meegotouch
CONFIG += qt \
    meegotouch \
    debug
QT += dbus
TARGET = $$qtLibraryTarget(connman-qt)
target.path = /usr/lib
!exists(manager.h) {
  system(qdbusxml2cpp -c Manager -p manager -N connman-manager.xml)
}

!exists(service.h) {
  system(qdbusxml2cpp -c Service -p service -N connman-service.xml)
}

HEADERS += manager.h \
    service.h \
    networkitemmodel.h \
    networklistmodel.h \
    commondbustypes.h \
    technologybutton.h \
    offlinebutton.h

head.files = manager.h service.h networkitemmodel.h \ 
	     networklistmodel.h commondbustypes.h technologybutton.h \
	     offlinebutton.h
head.path = /usr/include

pkgconfig.files = connman-qt.pc
pkgconfig.path = /usr/lib/pkgconfig

SOURCES += networkitemmodel.cpp \
           networklistmodel.cpp \
           technologybutton.cpp \
           manager.cpp \
	   service.cpp \
	   offlinebutton.cpp

INSTALLS += target head pkgconfig
