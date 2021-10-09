/**
 * @file        http_session.cpp
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

#include "http_session.h"
#include <gateway.h>
#include <http/file_send.h>

#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiCommon/common_methods/string_methods.h>
#include <libKitsunemimiCommon/logger.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>

using Kitsunemimi::Hanami::HanamiMessaging;

/**
 * @brief constructor
 */
HttpRequestEvent::HttpRequestEvent(tcp::socket &&socket,
                                   boost::asio::ssl::context &ctx)
    : m_socket(std::move(socket)),
      m_stream{m_socket, ctx} // the stupid template of the constructor for the variable m_stream
                              // doesn't accept the std::move function. Because result of std::move
                              // have to be written into it own variable first, before using this
                              // for the m_stream varible. So the order of these two is important!
{
    // Perform the SSL handshake
    beast::error_code ec;
    m_stream.handshake(boost::asio::ssl::stream_base::server, ec);
    if(ec.failed()) {
        LOG_ERROR("SSL-Handshake failed!");
    }
}

/**
 * @brief process http-session
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processEvent()
{
    if(readMessage() == false) {
        return false;
    }

    processRequest();
    m_request.clear();

    if(sendResponse() == false) {
        return false;
    }

    // close socket gain
    beast::error_code ec;
    m_stream.shutdown(ec);

    return true;
}

/**
 * @brief process request and build response
 */
void
HttpRequestEvent::processRequest()
{
    // build header for response
    m_response.version(m_request.version());
    m_response.keep_alive(false);
    m_response.set(http::field::server, "ToriiGateway");
    m_response.result(http::status::ok);
    m_response.set(http::field::content_type, "text/plain");

    // Request path must be absolute and not contain "..".
    if(m_request.target().empty()
            || m_request.target()[0] != '/'
            || m_request.target().find("..") != beast::string_view::npos)
    {
        // "Illegal request-target"
        m_response.result(http::status::bad_request);
        return;
    }

    switch(m_request.method())
    {
        case http::verb::get:
        {
            if(processGetRequest() == false) {
                m_response.result(http::status::not_found);
            }
            break;
        }

        case http::verb::post:
        {
            if(processPostRequest() == false) {
                m_response.result(http::status::not_found);
            }
            break;
        }

        case http::verb::put:
        {
            if(processPutRequest() == false) {
                m_response.result(http::status::not_found);
            }
            break;
        }

        case http::verb::delete_:
        {
            if(processDelesteRequest() == false) {
                m_response.result(http::status::not_found);
            }
            break;
        }

        default:
        {
            m_response.result(http::status::bad_request);
            const std::string errorMessage = "Invalid request-method '"
                                             + std::string(m_request.method_string())
                                             + "'";
            LOG_ERROR(errorMessage);
            beast::ostream(m_response.body()) << errorMessage;
            break;
        }
    }
}


/**
 * @brief send information of the websocket-connection
 *
 * @param client client-type (client, control, monitoring)
 * @param portName port-type (websocket_port, http_port)
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::sendConnectionInfo(const std::string &client,
                                     const std::string &portName)
{
    bool success = false;

    // get stuff from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG(client, portName, success));
    const std::string ip = GET_STRING_CONFIG("DEFAULT", "ip", success);

    // pack information into a response-message
    const std::string result = "{\"port\":"
                               + std::to_string(port)
                               + ",\"ip\":\""
                               + ip
                               + "\"}";
    m_response.set(http::field::content_type, "text/json");
    beast::ostream(m_response.body()) << result;

    return true;
}

/**
 * @brief read message into buffer
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::readMessage()
{
    beast::error_code ec;
    beast::flat_buffer buffer;
    http::read(m_stream, buffer, m_request, ec);

    if(ec == http::error::end_of_stream) {
         return true;
    }

    if(ec)
    {
        LOG_ERROR("read: " + ec.message());
        return false;
    }

    return true;
}

/**
 * @brief send prebuild response
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::sendResponse()
{
    beast::error_code ec;
    m_response.content_length(m_response.body().size());
    http::write(m_stream, m_response, ec);

    if(ec)
    {
        LOG_ERROR("write: " + ec.message());
        return false;
    }

    return true;
}

/**
 * @brief forward incoming get-requests
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processGetRequest()
{
    std::string path = m_request.target().to_string();

    if(path.compare(0, 7, "/client") == 0)
    {
        path.erase(0, 7);
        return processClientRequest(m_response, path);
    }

    if(path.compare(0, 9, "/control/") == 0)
    {
        path.erase(0, 9);
        processControlRequest(path, "{}");
        return true;
    }

    return false;
}

/**
 * @brief forward incoming post-requests
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processPostRequest()
{
    std::string path = m_request.target().to_string();

    if(path.compare(0, 9, "/control/") == 0)
    {
        path.erase(0, 9);
        processControlRequest(path, m_request.body().data());
        return true;
    }

    return false;
}

/**
 * @brief HttpRequestEvent::processPutRequest
 * @return
 */
bool
HttpRequestEvent::processPutRequest()
{
    return false;
}

/**
 * @brief HttpRequestEvent::processDelesteRequest
 * @return
 */
bool
HttpRequestEvent::processDelesteRequest()
{
    return false;
}

/**
 * @brief parseUri
 * @param path
 * @param inputValues
 * @param uri
 * @return
 */
bool
HttpRequestEvent::parseUri(std::string &path,
                           std::string &inputValues,
                           const std::string &uri)
{
    std::vector<std::string> parts;
    Kitsunemimi::splitStringByDelimiter(parts, uri, '?');

    Kitsunemimi::replaceSubstring(parts[1], "=", ":");
    Kitsunemimi::replaceSubstring(parts[1], "&", ",");

    path = parts.at(0);
    inputValues = "{" + parts.at(1) + "}";

    return true;
}

/**
 * @brief process control request by forwarding it to the backend
 *
 * @param path path to forward as identifier to trigger sakura file
 * @param inputValues json-formated input-values
 */
void
HttpRequestEvent::processControlRequest(const std::string &path,
                                        const std::string &inputValues)
{
    Kitsunemimi::DataMap result;
    std::string errorMessage = "";

    // trigger sakura-file remote
    bool ret = false;

    if(path.find("?") != std::string::npos)
    {
        std::string newPath;
        std::string inputValues;
        if(parseUri(newPath, inputValues, path))
        {
            ret = HanamiMessaging::getInstance()->triggerSakuraFile("KyoukoMind",
                                                                    result,
                                                                    newPath,
                                                                    inputValues,
                                                                    errorMessage);
        }
    }
    else
    {
        ret = HanamiMessaging::getInstance()->triggerSakuraFile("KyoukoMind",
                                                                result,
                                                                path,
                                                                inputValues,
                                                                errorMessage);
    }

    // forward result to the control
    if(ret)
    {
        m_response.result(http::status::ok);
        m_response.set(http::field::content_type, "text/json");
        beast::ostream(m_response.body()) << result.toString();
    }
    else
    {
        m_response.result(http::status::not_found);
        m_response.set(http::field::content_type, "text/plain");
        LOG_ERROR(errorMessage);
        beast::ostream(m_response.body()) << errorMessage;
    }
}
