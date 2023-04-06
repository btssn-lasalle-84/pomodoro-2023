QT       += core gui widgets sql bluetooth

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    Tache.cpp \
    basededonnees.cpp \
    cycle.cpp \
    pomodoro.cpp \
    main.cpp \
    session.cpp

HEADERS += \
    Tache.h \
    basededonnees.h \
    cycle.h \
    pomodoro.h \
    session.h

FORMS += \
    Cycle.ui \
    Session.ui \
    Tache.ui \
    pomodoro.ui


RESOURCES += \
    ressources.qrc

COPIES += bdd
bdd.files = pomodoro.sqlite
bdd.path = $$OUT_PWD/
bdd.base = $$PWD/

CONFIG(release, debug|release):DEFINES+=QT_NO_DEBUG_OUTPUT
