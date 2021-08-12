/**
 * @file        web_socket_server.h
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

    void setClientSession(WebSocketSession* session);

    WebSocketSession* getClientSession();

protected:
    void run();

private:
    std::string m_address = "";
    uint16_t m_port = 0;
    std::string m_type = "";

    std::atomic_flag m_clientSession_lock = ATOMIC_FLAG_INIT;
    WebSocketSession* m_activeClientSession = nullptr;
};

#endif // WEB_SOCKET_SERVER_H
