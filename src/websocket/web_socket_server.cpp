/**
 * @file        web_socket_server.cpp
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

#include "web_socket_server.h"

#include <websocket/web_socket_session.h>

#include <libKitsunemimiCommon/logger.h>

/**
 * @brief constructor
 *
 * @param address address to listen on
 * @param port port to listen
 * @param type type of the server (monitoring, client, control)
 */
WebSocketServer::WebSocketServer(const std::string &address,
                                 const uint16_t port)
    : Kitsunemimi::Thread("WebSocketServer"),
      m_address(address),
      m_port(port)
{}

/**
 * @brief run server-thread
 */
void
WebSocketServer::run()
{
    LOG_INFO("start Websocket-server on address "
             + m_address
             + " and port "
             + std::to_string(m_port));
    uint32_t counter = 0;
    try
    {
        // create server
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};

        while(m_abort == false)
        {
            // create socket-object for incoming connection
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            counter++;

            // initialize session
            const std::string name = "WebSocketSession_" + std::to_string(counter);
            WebSocketSession* session = new WebSocketSession(std::move(socket), name);
            const bool ret = session->initSessionToBackend("test");
            if(ret)
            {
                m_sessions.push_back(session);
                session->startThread();
            }
            else
            {
                delete session;
            }
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error in websocket-server with message: " + std::string(e.what()));
    }
}
