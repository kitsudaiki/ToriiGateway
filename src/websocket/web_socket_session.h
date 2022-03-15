/**
 * @file        web_socket_session.h
 *
 * @author      Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright   Apache License Version 2.0
 *
 *      Copyright 2019 Tobias Anker
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#ifndef TORIIGATEWAY_WEB_SOCKET_SESSION_H
#define TORIIGATEWAY_WEB_SOCKET_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/ssl.hpp>
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
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>

namespace Kitsunemimi {
namespace Hanami {
class HanamiMessaging;
}
}

using Kitsunemimi::Hanami::HanamiMessaging;

class WebSocketSession
        : public Kitsunemimi::Thread
{
public:
    WebSocketSession(beast::ssl_stream<tcp::socket&> &stream,
                     const std::string &threadName);

    bool init(http::request<http::string_body> &httpRequest);
    bool sendText(const std::string &text);

    bool isInit = false;

protected:
    void run();
    void closeSession();

    websocket::stream<beast::ssl_stream<tcp::socket&>> m_webSocket;

    std::string m_session = "";
};

#endif // TORIIGATEWAY_WEB_SOCKET_SESSION_H
