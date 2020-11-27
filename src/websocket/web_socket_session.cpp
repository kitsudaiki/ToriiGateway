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

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

WebSocketSession::WebSocketSession(tcp::socket &&socket, const std::string &type)
    : m_webSocket(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient(type);
}

bool
WebSocketSession::sendText(const std::string &text)
{
    beast::flat_buffer buffer;
    m_webSocket.text(true);
    boost::beast::ostream(buffer) << text;
    m_webSocket.write(buffer.data());

    return true;
}

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
        if(se.code() != websocket::error::closed) {
            std::cerr << "Error: " << se.code().message() << std::endl;
        }
        scheduleThreadForDeletion();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        scheduleThreadForDeletion();
    }
}
