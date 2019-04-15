QT -= qt core gui

TARGET = ToriiGateway
CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -L../libKitsuneCommon -lKitsuneCommon
LIBS += -L../libKitsuneCommon/debug -lKitsuneCommon
LIBS += -L../libKitsuneCommon/release -lKitsuneCommon
INCLUDEPATH += ../libKitsuneCommon/include/libKitsuneCommon

LIBS += -L../libKitsuneChanCommunication -lKitsuneChanCommunication
LIBS += -L../libKitsuneChanCommunication/debug -lKitsuneChanCommunication
LIBS += -L../libKitsuneChanCommunication/release -lKitsuneChanCommunication
INCLUDEPATH += ../libKitsuneChanCommunication/include/libKitsuneChanCommunication

LIBS += -L../libKitsuneNetwork/src -lKitsuneNetwork
LIBS += -L../libKitsuneNetwork/src/debug -lKitsuneNetwork
LIBS += -L../libKitsuneNetwork/src/release -lKitsuneNetwork
INCLUDEPATH += ../libKitsuneNetwork/include/libKitsuneNetwork

INCLUDEPATH += $$PWD \
               src

SOURCES += \
        src/main.cpp \
        src/connection.cpp

HEADERS += \
        src/connection.h
