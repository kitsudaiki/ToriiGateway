#ifndef MONITORING_WEB_SOCKET_SESSION_H
#define MONITORING_WEB_SOCKET_SESSION_H

#include <websocket/web_socket_session.h>

class MonitoringWebSocketSession
        : public WebSocketSession
{
public:
    MonitoringWebSocketSession(tcp::socket &&socket);
};

#endif // MONITORING_WEB_SOCKET_SESSION_H
