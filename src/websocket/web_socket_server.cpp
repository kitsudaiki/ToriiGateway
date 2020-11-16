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

#include <websocket/client_web_socket_session.h>
#include <websocket/monitoring_web_socket_session.h>

#include <libKitsunemimiPersistence/logger/logger.h>

WebSocketServer::WebSocketServer(const std::string &address,
                                 const uint16_t port,
                                 const std::string &type)
{
    m_address = address;
    m_port = port;
    m_type = type;
}

void
WebSocketServer::run()
{
    LOG_INFO("start Websocket-server on address " + m_address
             + " and port " + std::to_string(m_port));
    try
    {
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};

        while(m_abort == false)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            WebSocketSession* session = nullptr;
            if(m_type == "client")
            {
                session = new ClientWebSocketSession(std::move(socket));
                m_activeClientSessions.push_back(session);
            }
            if(m_type == "monitoring")
            {
                session = new MonitoringWebSocketSession(std::move(socket));
                m_activeMonitoringSessions.push_back(session);
            }

            session->startThread();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
