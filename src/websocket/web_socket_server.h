#ifndef WEB_SOCKET_SERVER_H
#define WEB_SOCKET_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

#include <libKitsunemimiCommon/threading/thread.h>

class WebSocketSession;

class WebSocketServer
        : public Kitsunemimi::Thread
{
public:
    WebSocketServer(const std::string &address,
                    const uint16_t port,
                    const std::string &type);

    std::vector<WebSocketSession*> m_activeClientSessions;
    std::vector<WebSocketSession*> m_activeMonitoringSessions;

protected:
    void run();

private:
    std::string m_address = "";
    uint16_t m_port = 0;
    std::string m_type = "";
};

#endif // WEB_SOCKET_SERVER_H
