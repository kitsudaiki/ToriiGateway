QT -= qt core gui

TARGET = ToriiGateway
CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -L../libKitsuneCommon/src -lKitsuneCommon
LIBS += -L../libKitsuneCommon/src/debug -lKitsuneCommon
LIBS += -L../libKitsuneCommon/src/release -lKitsuneCommon
INCLUDEPATH += ../libKitsuneCommon/include/libKitsuneCommon

LIBS += -L../libKitsuneChanCommon -lKitsuneChanCommon
LIBS += -L../libKitsuneChanCommon/debug -lKitsuneChanCommon
LIBS += -L../libKitsuneChanCommon/release -lKitsuneChanCommon
INCLUDEPATH += ../libKitsuneChanCommon/include/libKitsuneChanCommon

LIBS += -L../libKitsuneNetwork/src -lKitsuneNetwork
LIBS += -L../libKitsuneNetwork/src/debug -lKitsuneNetwork
LIBS += -L../libKitsuneNetwork/src/release -lKitsuneNetwork
INCLUDEPATH += ../libKitsuneNetwork/include/libKitsuneNetwork

INCLUDEPATH += $$PWD \
               src

SOURCES += \
        src/main.cpp \
    src/gateway_server.cpp \
    src/gateway_server_trigger.cpp

HEADERS += \
    src/gateway_server.h \
    src/gateway_server_trigger.h
