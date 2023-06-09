QT       += core gui widgets sql bluetooth
QT += bluetooth

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    basededonnees.cpp \
    bluetoothpomodoro.cpp \
    peripherique.cpp \
    pomodoro.cpp \
    main.cpp

HEADERS += \
    basededonnees.h \
    bluetoothpomodoro.h \
    peripherique.h \
    pomodoro.h

FORMS += \
    pomodoro.ui


RESOURCES += \
    ressources.qrc

COPIES += bdd
bdd.files = pomodoro.sqlite
bdd.path = $$OUT_PWD/
bdd.base = $$PWD/

CONFIG(release, debug|release):DEFINES+=QT_NO_DEBUG_OUTPUT
