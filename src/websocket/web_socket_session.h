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
    WebSocketSession(tcp::socket &&socket, const std::string &threadName);

    bool initSessionToBackend(const std::string &identifier);

    bool sendText(const std::string &text);

protected:
    void run();
    void closeSession();

    websocket::stream<beast::tcp_stream> m_webSocket;
    std::string m_session = "";
};

#endif // WEB_SOCKET_SESSION_H
