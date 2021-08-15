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
#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiPersistence/logger/logger.h>

using Kitsunemimi::Sakura::MessagingController;

/**
 * @brief constructor
 */
WebSocketSession::WebSocketSession(tcp::socket &&socket)
    : m_webSocket(std::move(socket))
{
}

/**
 * @brief WebSocketSession::initSessionToBackend
 * @return
 */
bool
WebSocketSession::initSessionToBackend(const std::string &identifier)
{
    bool success = false;

    const std::string address = GET_STRING_CONFIG("KyoukoMind", "address", success);
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("KyoukoMind", "port", success));

    MessagingController* contr = MessagingController::getInstance();
    m_client = contr->createClient(identifier, identifier, address, port);

    if(m_client == nullptr) {
        return false;
    }

    return true;
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
            LOG_INFO("Close websocket");
            closeSession();
        }
        else
        {
            LOG_ERROR("Error while sending data over websocket with message: "
                      + se.code().message());
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("Error while sending data over websocket with message: " + std::string(e.what()));
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
            LOG_INFO("Close websocket");
            closeSession();
        }
        else
        {
            LOG_ERROR("Error while receiving data over websocket with message: "
                      + se.code().message());
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("Error while receiving data over websocket with message: "
                  + std::string(e.what()));
    }
}

/**
 * @brief close session
 */
void
WebSocketSession::closeSession()
{
    m_abort = true;

    WebSocketServer* server = Gateway::m_instance->m_websocketServer;

    scheduleThreadForDeletion();
}
