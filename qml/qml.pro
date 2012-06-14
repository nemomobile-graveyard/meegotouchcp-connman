TEMPLATE = subdirs

#FIXME: should not hardcode
desktop_entry.path = /usr/lib/duicontrolpanel
desktop_entry.files = meegotouchcp-wifi.desktop

qmlpages.path = /usr/share/duicontrolpanel/wifi
qmlpages.files = mainpage.qml

mustache.path = /usr/lib/qt4/imports/Connman/js/
mustache.files = mustache.js

INSTALLS += desktop_entry qmlpages mustache
