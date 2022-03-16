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

#include <torii_root.h>
#include <core/http_processing/http_processing.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>
#include <libKitsunemimiHanamiMessaging/hanami_messaging_client.h>
#include <libKitsunemimiHanamiCommon/enums.h>
#include <libKitsunemimiHanamiCommon/structs.h>

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiJson/json_item.h>
#include <libKitsunemimiCommon/logger.h>

using Kitsunemimi::Hanami::HanamiMessaging;
using Kitsunemimi::Hanami::HttpRequestType;
using Kitsunemimi::Hanami::HttpResponseTypes;
using namespace Kitsunemimi;

/**
 * @brief constructor
 */
WebSocketSession::WebSocketSession(beast::ssl_stream<tcp::socket&>* stream,
                                   const std::string &threadName)
    : Kitsunemimi::Thread(threadName),
      m_webSocket(std::move(*stream)),
      m_stream(stream)
{
}

bool
WebSocketSession::init(http::request<http::string_body>* httpRequest)
{
    m_httpRequest = httpRequest;
    try
    {
        // Set a decorator to change the Server of the handshake
        m_webSocket.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " torii-websocket-ssl");
            }));

        // Accept the websocket handshake
        m_webSocket.accept(std::move(*m_httpRequest));
    }
    catch(const beast::system_error& se)
    {
        if(se.code() == websocket::error::closed)
        {
            LOG_INFO("Close websocket");
        }
        else
        {
            Kitsunemimi::ErrorContainer error;
            error.addMeesage("Error while receiving data over websocket with message: "
                             + se.code().message());
            LOG_ERROR(error);
            return false;
        }
    }
    catch(const std::exception& e)
    {
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("Error while receiving data over websocket with message: "
                         + std::string(e.what()));
        LOG_ERROR(error);
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
WebSocketSession::sendText(const void* data,
                           const uint64_t dataSize)
{
    if(m_abort) {
        return false;
    }

    try
    {
        beast::flat_buffer buffer;
        m_webSocket.binary(true);
        m_webSocket.write(net::buffer(data, dataSize));
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
            Kitsunemimi::ErrorContainer error;
            error.addMeesage("Error while sending data over websocket with message: "
                             + se.code().message());
            LOG_ERROR(error);
        }
    }
    catch(const std::exception& e)
    {
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("Error while sending data over websocket with message: "
                         + std::string(e.what()));
        LOG_ERROR(error);
    }

    return false;
}

bool
WebSocketSession::processInitialMessage(const std::string &message,
                                        Kitsunemimi::ErrorContainer &error)
{
    Hanami::HanamiMessaging* messageInterface = Hanami::HanamiMessaging::getInstance();
    Kitsunemimi::Json::JsonItem content;
    if(content.parse(message, error) == false) {
        return false;
    }

    const std::string target = content.get("target").getString();

    Kitsunemimi::Hanami::RequestMessage requestMsg;
    Kitsunemimi::Hanami::ResponseMessage responseMsg;

    requestMsg.id = "v1/foreward";
    requestMsg.httpType = HttpRequestType::GET_TYPE;

    // check authentication
    if(checkPermission(content.get("token").getString(),
                       target,
                       requestMsg,
                       responseMsg,
                       error) == false)
    {
        return false;
    }

    // handle failed authentication
    if(responseMsg.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE
            || responseMsg.success == false)
    {
        return false;
    }

    // get target-session by name
    m_session = messageInterface->getOutgoingClient(target);
    if(m_session == nullptr) {
        return false;
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
        isInit = true;
        Kitsunemimi::ErrorContainer error;

        while(m_abort == false)
        {
            // read message from socket
            beast::flat_buffer buffer;
            m_webSocket.read(buffer);

            //m_webSocket.text(m_webSocket.got_text());
            if(m_session == nullptr)
            {
                const std::string msg(static_cast<const char*>(buffer.data().data()),
                                      buffer.data().size());
                LOG_DEBUG("got initial websocket-message: '" + msg + "'");
                std::string response = "success";
                if(processInitialMessage(msg, error) == false)  {
                    response = "failed";
                }

                m_webSocket.binary(true);
                m_webSocket.write(net::buffer(response, response.size()));
            }
            else
            {
                m_session->sendStreamMessage(buffer.data().data(),
                                             buffer.data().size(),
                                             false,
                                             error);
            }
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
            Kitsunemimi::ErrorContainer error;
            error.addMeesage("Error while receiving data over websocket with message: "
                             + se.code().message());
            LOG_ERROR(error);
        }
    }
    catch(const std::exception& e)
    {
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("Error while receiving data over websocket with message: "
                         + std::string(e.what()));
        LOG_ERROR(error);
    }
}

/**
 * @brief close session
 */
void
WebSocketSession::closeSession()
{
    m_abort = true;

    scheduleThreadForDeletion();
}
