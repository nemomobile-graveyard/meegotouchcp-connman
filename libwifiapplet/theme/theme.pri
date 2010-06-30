#-*-Shell-Script-*-

load(meegotouch_defines)
include(/usr/lib/qt4/mkspecs/dcpconfig.pri)


# icons.files = $$system(find $$PWD/icons -type f)
# icons.path = $$DCP_THEME_DIR/icons

# svg.files = $$system(find $$PWD/svg -type f)
# svg.path = $$DCP_THEME_DIR/svg

images.files = $$system(find $$PWD/images -type f)
images.path = $$DCP_THEME_DIR/images

style.files = $$system(find $$PWD/style -type f -name \*.css)
style.path = $$DCP_THEME_DIR/style

conf.files = $$system(find $$PWD -type f -name \*.conf)
conf.path = $$M_THEME_DIR/base/meegotouch/libwifiapplet/

OTHER_FILES += $$system(find $$PWD/style -type f -name \*.css)

INSTALLS += style images conf
