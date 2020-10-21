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

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebSocketSession;

class WebSocketServer
        : public Kitsunemimi::Thread
{
public:
    WebSocketServer(const std::string &address,
                    const uint16_t port);

protected:
    void run();

private:
    std::string m_address = "";
    uint16_t m_port = 0;

    std::vector<WebSocketSession*> m_activeSession;
};

#endif // WEB_SOCKET_SERVER_H
