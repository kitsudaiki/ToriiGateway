#ifndef CLIENT_WEB_SOCKET_SESSION_H
#define CLIENT_WEB_SOCKET_SESSION_H

#include <websocket/web_socket_session.h>

class ClientWebSocketSession
        : public WebSocketSession
{
public:
    ClientWebSocketSession(tcp::socket &&socket);
};

#endif // CLIENT_WEB_SOCKET_SESSION_H
