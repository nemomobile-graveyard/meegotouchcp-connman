#-*-Shell-Script-*-

include (common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += libconnman-qt libwifiapplet translations

#FIXME: should not hardcode
desktop_entry.path = /usr/lib/duicontrolpanel
desktop_entry.files = meegotouchcp-connman.desktop

INSTALLS += desktop_entry
