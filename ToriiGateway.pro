QT -= qt core gui

TARGET = ToriiGateway
CONFIG += console c++14
CONFIG -= app_bundle

LIBS += -L../libKitsunemimiArgs/src -lKitsunemimiArgs
LIBS += -L../libKitsunemimiArgs/src/debug -lKitsunemimiArgs
LIBS += -L../libKitsunemimiArgs/src/release -lKitsunemimiArgs
INCLUDEPATH += ../libKitsunemimiArgs/include

LIBS += -L../libKitsunemimiConfig/src -lKitsunemimiConfig
LIBS += -L../libKitsunemimiConfig/src/debug -lKitsunemimiConfig
LIBS += -L../libKitsunemimiConfig/src/release -lKitsunemimiConfig
INCLUDEPATH += ../libKitsunemimiConfig/include

LIBS += -L../libKitsunemimiKyoukoNetwork/src -lKitsunemimiKyoukoNetwork
LIBS += -L../libKitsunemimiKyoukoNetwork/src/debug -lKitsunemimiKyoukoNetwork
LIBS += -L../libKitsunemimiKyoukoNetwork/src/release -lKitsunemimiKyoukoNetwork
INCLUDEPATH += ../libKitsunemimiKyoukoNetwork/include

LIBS += -L../libKitsunemimiPersistence/src -lKitsunemimiPersistence
LIBS += -L../libKitsunemimiPersistence/src/debug -lKitsunemimiPersistence
LIBS += -L../libKitsunemimiPersistence/src/release -lKitsunemimiPersistence
INCLUDEPATH += ../libKitsunemimiPersistence/include

LIBS += -L../libKitsunemimiProjectNetwork/src -lKitsunemimiProjectNetwork
LIBS += -L../libKitsunemimiProjectNetwork/src/debug -lKitsunemimiProjectNetwork
LIBS += -L../libKitsunemimiProjectNetwork/src/release -lKitsunemimiProjectNetwork
INCLUDEPATH += ../libKitsunemimiProjectNetwork/include

LIBS += -L../libKitsunemimiNetwork/src -lKitsunemimiNetwork
LIBS += -L../libKitsunemimiNetwork/src/debug -lKitsunemimiNetwork
LIBS += -L../libKitsunemimiNetwork/src/release -lKitsunemimiNetwork
INCLUDEPATH += ../libKitsunemimiNetwork/include

LIBS += -L../libKitsunemimiCommon/src -lKitsunemimiCommon
LIBS += -L../libKitsunemimiCommon/src/debug -lKitsunemimiCommon
LIBS += -L../libKitsunemimiCommon/src/release -lKitsunemimiCommon
INCLUDEPATH += ../libKitsunemimiCommon/include


LIBS +=  -lssl -lcrypt -lboost_filesystem -lboost_system

INCLUDEPATH += $$PWD \
               src

SOURCES += \
        src/main.cpp \
    src/gateway.cpp

HEADERS += \
    src/gateway.h \
    src/config.h \
    src/args.h
