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

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiPersistence/files/text_file.h>
#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

/**
 * @brief constructor
 */
HttpSession::HttpSession(tcp::socket &&socket)
    : m_socket(std::move(socket))
{
}

/**
 * @brief process http-session
 *
 * @return true, if successful, else false
 */
bool
HttpSession::processEvent()
{
    if(readMessage() == false) {
        return false;
    }

    processRequest();

    if(sendResponse() == false) {
        return false;
    }

    // close socket gain
    beast::error_code ec;
    m_socket.shutdown(tcp::socket::shutdown_send, ec);

    return true;
}

/**
 * @brief process request and build response
 */
void
HttpSession::processRequest()
{
    m_response.version(m_request.version());
    m_response.keep_alive(false);
    m_response.set(http::field::server, "ToriiGateway");
    m_response.result(http::status::ok);

    switch(m_request.method())
    {
        case http::verb::get:
        {
            if(processGetRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::post:
        {
            if(processPostRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::put:
        {
            if(processPutRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::delete_:
        {
            if(processDelesteRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        default:
        {
            m_response.result(http::status::bad_request);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body())
                << "Invalid request-method '"
                << std::string(m_request.method_string())
                << "'";
            break;
        }
    }
}

/**
 * @brief send file, which was requested
 *
 * @return true, if successful, else false
 */
bool
HttpSession::sendFileFromLocalLocation()
{
    // create file-path
    std::string path = m_fileLocation;
    path += m_request.target().to_string();
    if(m_request.target() == "/") {
        path += "index.html";
    }

    // set response-type based on file-type
    boost::filesystem::path pathObj(path);
    const std::string extension = pathObj.extension().string();
    if(pathObj.extension().string() == ".html") {
        m_response.set(http::field::content_type, "text/html");
    } else if(pathObj.extension().string() == ".css") {
        m_response.set(http::field::content_type, "text/css");
    } else if(pathObj.extension().string() == ".js") {
        m_response.set(http::field::content_type, "text/javascript");
    } else {
        m_response.set(http::field::content_type, "text/plain");
    }

    // read file and send content back
    std::string fileContent = "";
    std::string errorMessage = "";
    if(Kitsunemimi::Persistence::readFile(fileContent, path, errorMessage))
    {
        beast::ostream(m_response.body()) << fileContent;
        return true;
    }

    m_response.result(http::status::internal_server_error);
    m_response.set(http::field::content_type, "text/plain");
    LOG_ERROR(errorMessage);

    return false;
}

/**
 * @brief send information of the websocket-connection
 *
 * @param client client-type (control, monitoring)
 * @param portName port-type (websocket_port, http_port)
 *
 * @return true, if successful, else false
 */
bool
HttpSession::sendConnectionInfo(const std::string &client,
                                const std::string &portName)
{
    bool success = false;

    // get port of websocket from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG(client, portName, success));
    if(success == false) {
        return false;
    }

    // get ip of websocket from config
    const std::string ip = GET_STRING_CONFIG(client, "ip", success);
    if(success == false) {
        return false;
    }

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
HttpSession::readMessage()
{
    beast::error_code ec;
    beast::flat_buffer buffer;
    http::read(m_socket, buffer, m_request, ec);

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
HttpSession::sendResponse()
{
    beast::error_code ec;
    m_response.content_length(m_response.body().size());
    http::write(m_socket, m_response, ec);

    if(ec)
    {
        LOG_ERROR("write: " + ec.message());
        return false;
    }

    return true;
}
