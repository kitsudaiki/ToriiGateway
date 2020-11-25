QT -= qt core gui

TARGET = ToriiGateway
CONFIG += console c++14
CONFIG -= app_bundle


LIBS += -L../libKitsunemimiSakuraMessaging/src -lKitsunemimiSakuraMessaging
LIBS += -L../libKitsunemimiSakuraMessaging/src/debug -lKitsunemimiSakuraMessaging
LIBS += -L../libKitsunemimiSakuraMessaging/src/release -lKitsunemimiSakuraMessaging
INCLUDEPATH += ../libKitsunemimiSakuraMessaging/include

LIBS += -L../libKitsunemimiSakuraLang/src -lKitsunemimiSakuraLang
LIBS += -L../libKitsunemimiSakuraLang/src/debug -lKitsunemimiSakuraLang
LIBS += -L../libKitsunemimiSakuraLang/src/release -lKitsunemimiSakuraLang
INCLUDEPATH += ../libKitsunemimiSakuraLang/include

LIBS += -L../libKitsunemimiArgs/src -lKitsunemimiArgs
LIBS += -L../libKitsunemimiArgs/src/debug -lKitsunemimiArgs
LIBS += -L../libKitsunemimiArgs/src/release -lKitsunemimiArgs
INCLUDEPATH += ../libKitsunemimiArgs/include

LIBS += -L../libKitsunemimiConfig/src -lKitsunemimiConfig
LIBS += -L../libKitsunemimiConfig/src/debug -lKitsunemimiConfig
LIBS += -L../libKitsunemimiConfig/src/release -lKitsunemimiConfig
INCLUDEPATH += ../libKitsunemimiConfig/include

LIBS += -L../libKitsunemimiSakuraNetwork/src -lKitsunemimiSakuraNetwork
LIBS += -L../libKitsunemimiSakuraNetwork/src/debug -lKitsunemimiSakuraNetwork
LIBS += -L../libKitsunemimiSakuraNetwork/src/release -lKitsunemimiSakuraNetwork
INCLUDEPATH += ../libKitsunemimiSakuraNetwork/include

LIBS += -L../libKitsunemimiSakuraNetwork/src -lKitsunemimiSakuraNetwork
LIBS += -L../libKitsunemimiSakuraNetwork/src/debug -lKitsunemimiSakuraNetwork
LIBS += -L../libKitsunemimiSakuraNetwork/src/release -lKitsunemimiSakuraNetwork
INCLUDEPATH += ../libKitsunemimiSakuraNetwork/include

LIBS += -L../libKitsunemimiCommon/src -lKitsunemimiCommon
LIBS += -L../libKitsunemimiCommon/src/debug -lKitsunemimiCommon
LIBS += -L../libKitsunemimiCommon/src/release -lKitsunemimiCommon
INCLUDEPATH += ../libKitsunemimiCommon/include

LIBS += -L../libKitsunemimiNetwork/src -lKitsunemimiNetwork
LIBS += -L../libKitsunemimiNetwork/src/debug -lKitsunemimiNetwork
LIBS += -L../libKitsunemimiNetwork/src/release -lKitsunemimiNetwork
INCLUDEPATH += ../libKitsunemimiNetwork/include

LIBS += -L../libKitsunemimiPersistence/src -lKitsunemimiPersistence
LIBS += -L../libKitsunemimiPersistence/src/debug -lKitsunemimiPersistence
LIBS += -L../libKitsunemimiPersistence/src/release -lKitsunemimiPersistence
INCLUDEPATH += ../libKitsunemimiPersistence/include

LIBS += -L../libKitsunemimiJinja2/src -lKitsunemimiJinja2
LIBS += -L../libKitsunemimiJinja2/src/debug -lKitsunemimiJinja2
LIBS += -L../libKitsunemimiJinja2/src/release -lKitsunemimiJinja2
INCLUDEPATH += ../libKitsunemimiJinja2/include

LIBS += -L../libKitsunemimiJson/src -lKitsunemimiJson
LIBS += -L../libKitsunemimiJson/src/debug -lKitsunemimiJson
LIBS += -L../libKitsunemimiJson/src/release -lKitsunemimiJson
INCLUDEPATH += ../libKitsunemimiJson/include

LIBS += -L../libKitsunemimiIni/src -lKitsunemimiIni
LIBS += -L../libKitsunemimiIni/src/debug -lKitsunemimiIni
LIBS += -L../libKitsunemimiIni/src/release -lKitsunemimiIni
INCLUDEPATH += ../libKitsunemimiIni/include

LIBS += -lpthread -lboost_filesystem -lboost_system -lcrypto -lssl

INCLUDEPATH += $$PWD \
               src

SOURCES += \
        src/main.cpp \
        src/gateway.cpp \
    src/websocket/web_socket_server.cpp \
    src/websocket/web_socket_session.cpp \
    src/http/http_server.cpp \
    src/http/http_session.cpp \
    src/http/client_http_session.cpp \
    src/http/monitoring_http_session.cpp \
    src/http/control_http_session.cpp \
    src/websocket/client_web_socket_session.cpp \
    src/websocket/monitoring_web_socket_session.cpp

HEADERS += \
    src/gateway.h \
    src/args.h \
    src/websocket/web_socket_server.h \
    src/websocket/web_socket_session.h \
    src/http/http_server.h \
    src/http/http_session.h \
    src/config.h \
    src/http/client_http_session.h \
    src/http/monitoring_http_session.h \
    src/http/control_http_session.h \
    src/websocket/client_web_socket_session.h \
    src/websocket/monitoring_web_socket_session.h \
    src/callbacks.h
