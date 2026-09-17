QT       += widgets
TEMPLATE  = lib
CONFIG   += plugin c++17
TARGET    = testplugin

SOURCES += testplugin.cpp
HEADERS += testplugin.h PluginInterface.h

# Optional: gebaute Plugin-Datei direkt in einen "plugins"-Ordner
# neben der Hauptanwendung legen. Pfad bei Bedarf anpassen.
DESTDIR = $$PWD/../plugins