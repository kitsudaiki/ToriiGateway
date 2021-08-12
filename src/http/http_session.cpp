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

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiPersistence/files/text_file.h>
#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiSakuraMessaging/messaging_client.h>

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
    if(ec) {
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
 * @brief HttpRequestEvent::getResponseType
 * @param ext
 * @return
 */
const std::string
HttpRequestEvent::getResponseType(const std::string &ext)
{
    if(beast::iequals(ext, ".htm"))  { return "text/html"; }
    if(beast::iequals(ext, ".html")) { return "text/html"; }
    if(beast::iequals(ext, ".php"))  { return "text/html"; }
    if(beast::iequals(ext, ".css"))  { return "text/css"; }
    if(beast::iequals(ext, ".txt"))  { return "text/plain"; }
    if(beast::iequals(ext, ".js"))   { return "application/javascript"; }
    if(beast::iequals(ext, ".json")) { return "application/json"; }
    if(beast::iequals(ext, ".xml"))  { return "application/xml"; }
    if(beast::iequals(ext, ".swf"))  { return "application/x-shockwave-flash"; }
    if(beast::iequals(ext, ".flv"))  { return "video/x-flv"; }
    if(beast::iequals(ext, ".png"))  { return "image/png"; }
    if(beast::iequals(ext, ".jpe"))  { return "image/jpeg"; }
    if(beast::iequals(ext, ".jpeg")) { return "image/jpeg"; }
    if(beast::iequals(ext, ".jpg"))  { return "image/jpeg"; }
    if(beast::iequals(ext, ".gif"))  { return "image/gif"; }
    if(beast::iequals(ext, ".bmp"))  { return "image/bmp"; }
    if(beast::iequals(ext, ".ico"))  { return "image/vnd.microsoft.icon"; }
    if(beast::iequals(ext, ".tiff")) { return "image/tiff"; }
    if(beast::iequals(ext, ".tif"))  { return "image/tiff"; }
    if(beast::iequals(ext, ".svg"))  { return "image/svg+xml"; }
    if(beast::iequals(ext, ".svgz")) { return "image/svg+xml"; }

    return "application/text";
}

/**
 * @brief send file, which was requested
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::sendFileFromLocalLocation(const std::string &dir,
                                            const std::string &relativePath)
{
    // create file-path
    std::string path = dir + relativePath;
    if(relativePath == "/"){
        path += "index.html";
    }
    if(relativePath == ""){
        path += "/index.html";
    }

    LOG_DEBUG("load file " + path);

    // set response-type based on file-type
    boost::filesystem::path pathObj(path);
    const std::string extension = pathObj.extension().string();
    m_response.set(http::field::content_type, getResponseType(extension));

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

    // get port from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG(client, portName, success));
    if(success == false) {
        return false;
    }

    // get ip from config
    const std::string ip = GET_STRING_CONFIG("DEFAULT", "ip", success);
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
        return processClientRequest(path);
    }

    if(path.compare(0, 11, "/monitoring") == 0)
    {
        path.erase(0, 11);
        return processMonitoringRequest(path);
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
    std::string path = m_request.target().to_string();

    if(path.compare(0, 8, "/control") == 0)
    {
        path.erase(0, 8);
        return false;
    }

    return false;
}

/**
 * @brief HttpRequestEvent::processDelesteRequest
 * @return
 */
bool
HttpRequestEvent::processDelesteRequest()
{
    std::string path = m_request.target().to_string();

    if(path.compare(0, 8, "/control") == 0)
    {
        path.erase(0, 8);
        return false;
    }

    return false;
}

/**
 * @brief HttpRequestEvent::processClientRequest
 * @param path
 * @return
 */
bool
HttpRequestEvent::processClientRequest(const std::string &path)
{
    bool success = false;
    const std::string fileLocation = GET_STRING_CONFIG("client", "location", success);
    if(success == false) {
        return false;
    }

    if(path == "/websocket") {
        return sendConnectionInfo("client", "websocket_port");
    } else {
        sendFileFromLocalLocation(fileLocation, path);
    }

    return true;
}

/**
 * @brief get file of
 *
 * @param path requested path
 *
 * @return true, f
 */
bool
HttpRequestEvent::processMonitoringRequest(const std::string &path)
{
    bool success = false;
    const std::string fileLocation = GET_STRING_CONFIG("monitoring", "location", success);
    if(success == false) {
        return false;
    }

    if(path == "/websocket") {
        return sendConnectionInfo("monitoring", "websocket_port");
    } else {
        sendFileFromLocalLocation(fileLocation, path);
    }

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
    Kitsunemimi::Sakura::MessagingClient* m_client = Gateway::m_instance->getClient("control");

    Kitsunemimi::DataMap result;
    std::string errorMessage = "";

    // trigger sakura-file remote
    const bool ret = m_client->triggerSakuraFile(result, path, inputValues, errorMessage);

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
