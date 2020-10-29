#ifndef WEB_SOCKET_SESSION_H
#define WEB_SOCKET_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <random>

#include <libKitsunemimiCommon/threading/thread.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;

class WebSocketSession
        : public Kitsunemimi::Thread
{
public:
    WebSocketSession(tcp::socket &&socket);

protected:
    void run();

private:
    websocket::stream<beast::tcp_stream> m_webSocket;
};

#endif // WEB_SOCKET_SESSION_H
