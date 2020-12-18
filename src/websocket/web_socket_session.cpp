/**
 * @file        web_socket_session.cpp
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

#include "web_socket_session.h"

#include <gateway.h>
#include <websocket/web_socket_server.h>

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

#include <libKitsunemimiPersistence/logger/logger.h>

/**
 * @brief constructor
 */
WebSocketSession::WebSocketSession(tcp::socket &&socket, const std::string &type)
    : m_webSocket(std::move(socket))
{
    m_type = type;
    m_client = Gateway::m_instance->getClient(type);
}

/**
 * @brief send text over the websocket
 *
 * @param text text to send
 *
 * @return true, if successful, else false
 */
bool
WebSocketSession::sendText(const std::string &text)
{
    if(m_abort) {
        return false;
    }

    beast::flat_buffer buffer;
    m_webSocket.text(true);
    try
    {
        boost::beast::ostream(buffer) << text;
        return m_webSocket.write(buffer.data());
    }
    catch(const beast::system_error& se)
    {
        if(se.code() == websocket::error::closed)
        {
            LOG_INFO("Close websocket of type " + m_type);
            closeSession();
        }
        else
        {
            LOG_ERROR("Error while sending data over websocket of type " + m_type
                      + " with message: " + se.code().message());
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("Error while sending data over websocket of type " + m_type
                  + " with message: " + e.what());
    }

    return false;
}

/**
 * @brief run session-thread to process incoming messages from the websocket
 */
void
WebSocketSession::run()
{
    try
    {
        // Set a decorator to change the Server of the handshake
        m_webSocket.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");
            }));

        // Accept the websocket handshake
        m_webSocket.accept();

        while(m_abort == false)
        {
            beast::flat_buffer buffer;
            m_webSocket.read(buffer);
            m_client->sendStreamData(buffer.data().data(), buffer.data().size());
        }
    }
    catch(const beast::system_error& se)
    {
        if(se.code() == websocket::error::closed)
        {
            LOG_INFO("Close websocket of type " + m_type);
            closeSession();
        }
        else
        {
            LOG_ERROR("Error while receiving data over websocket of type " + m_type
                      + " with message: " + se.code().message());
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("Error while receiving data over websocket of type " + m_type
                  + " with message: " + e.what());
    }
}

/**
 * @brief close session
 */
void
WebSocketSession::closeSession()
{
    m_abort = true;

    Gateway* gateway = Gateway::m_instance;

    if(m_type == "client")
    {
        WebSocketServer* server = gateway->m_clientWebsocketServer;
        server->setClientSession(nullptr);
    }
    if(m_type == "monitoring")
    {
        WebSocketServer* server = gateway->m_monitoringWebsocketServer;
        server->setMonitoringSession(nullptr);
    }

    scheduleThreadForDeletion();
}
