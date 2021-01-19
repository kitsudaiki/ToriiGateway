/**
 * @file        http_server.cpp
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

#include "http_server.h"

#include <http/http_session.h>
#include <http/http_thread.h>

#include <libKitsunemimiPersistence/logger/logger.h>

/**
 * @brief constructor
 *
 * @param address address to listen on
 * @param port port to listen
 * @param type type of the server (monitoring, client, control)
 */
HttpServer::HttpServer(const std::string &address,
                       const uint16_t port)
{
    m_address = address;
    m_port = port;

    m_httpThread = new HttpThread();
    m_httpThread->startThread();
}

/**
 * @brief run server-thread
 */
void
HttpServer::run()
{
    LOG_INFO("start HTTP-server on address " + m_address + " and port " + std::to_string(m_port));
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

            // process http-request within an already existing thread
            HttpRequestEvent* event = new HttpRequestEvent(std::move(socket));
            m_httpThread->addEventToQueue(event);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
