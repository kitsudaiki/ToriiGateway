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
#include <http/response_builds.h>

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiJson/json_item.h>

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
    if(ec.failed())
    {
        Kitsunemimi::ErrorContainer error;
        error.errorMessage = "SSL-Handshake failed!";
        LOG_ERROR(error);
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
    m_httpRequest.clear();

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
    // build default-header for response
    m_httpResponse.version(m_httpRequest.version());
    m_httpResponse.keep_alive(false);
    m_httpResponse.set(http::field::server, "ToriiGateway");
    m_httpResponse.result(http::status::ok);
    m_httpResponse.set(http::field::content_type, "text/plain");

    // Request path must be absolute and not contain "..".
    if(m_httpRequest.target().empty()
            || m_httpRequest.target()[0] != '/'
            || m_httpRequest.target().find("..") != beast::string_view::npos)
    {
        // "Illegal request-target"
        m_httpResponse.result(http::status::bad_request);
        return;
    }

    std::string path = m_httpRequest.target().to_string();
    switch(m_httpRequest.method())
    {
        case http::verb::get:
        {
            processGetRequest(path);
            break;
        }

        case http::verb::post:
        {
            processPostRequest(path);
            break;
        }

        case http::verb::put:
        {
            processPutRequest(path);
            break;
        }

        case http::verb::delete_:
        {
            processDelesteRequest(path);
            break;
        }

        default:
        {
            m_httpResponse.result(http::status::bad_request);
            Kitsunemimi::ErrorContainer error;
            error.errorMessage = "Invalid request-method '"
                                 + std::string(m_httpRequest.method_string())
                                 + "'";
            LOG_ERROR(error);
            beast::ostream(m_httpResponse.body()) << error.errorMessage;
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
    m_httpResponse.set(http::field::content_type, "text/json");
    beast::ostream(m_httpResponse.body()) << result;

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
    http::read(m_stream, buffer, m_httpRequest, ec);

    if(ec == http::error::end_of_stream) {
         return true;
    }

    if(ec)
    {
        Kitsunemimi::ErrorContainer error;
        error.errorMessage = "read: " + ec.message();
        LOG_ERROR(error);
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
    m_httpResponse.content_length(m_httpResponse.body().size());
    http::write(m_stream, m_httpResponse, ec);

    if(ec)
    {
        Kitsunemimi::ErrorContainer error;
        error.errorMessage = "write: " + ec.message();
        LOG_ERROR(error);
        return false;
    }

    return true;
}

bool
HttpRequestEvent::cutPath(std::string &path, const std::string &cut)
{
    if(path.compare(0, cut.size(), cut) == 0)
    {
        path.erase(0, cut.size());
        return true;
    }

    return false;
}

/**
 * @brief forward incoming get-requests
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processGetRequest(std::string &path)
{
    if(cutPath(path, "/client")) {
        return processClientRequest(m_httpResponse, path);
    }

    if(cutPath(path, "/control/")) {
        return processControlRequest(path, "{}", HttpRequestType::GET_TYPE);
    }

    genericError_ResponseBuild(m_httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               "no matchin endpoint");
    return false;
}

/**
 * @brief forward incoming post-requests
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processPostRequest(std::string &path)
{
    if(cutPath(path, "/control/")) {
        return processControlRequest(path, m_httpRequest.body().data(), HttpRequestType::POST_TYPE);
    }


    genericError_ResponseBuild(m_httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               "no matchin endpoint");
    return false;}

/**
 * @brief HttpRequestEvent::processPutRequest
 * @return
 */
bool
HttpRequestEvent::processPutRequest(std::string &path)
{
    if(cutPath(path, "/control/")) {
        return processControlRequest(path, m_httpRequest.body().data(), HttpRequestType::PUT_TYPE);
    }


    genericError_ResponseBuild(m_httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               "no matchin endpoint");
    return false;}

/**
 * @brief HttpRequestEvent::processDelesteRequest
 * @return
 */
bool
HttpRequestEvent::processDelesteRequest(std::string &path)
{
    if(cutPath(path, "/control/")) {
        return processControlRequest(path, "{}", HttpRequestType::DELETE_TYPE);
    }


    genericError_ResponseBuild(m_httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               "no matchin endpoint");
    return false;
}

/**
 * @brief HttpRequestEvent::requestToken
 * @param target
 * @param hanamiRequest
 * @param hanamiResponse
 * @param errorMessage
 * @return
 */
bool
HttpRequestEvent::requestToken(const std::string &target,
                               Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                               Kitsunemimi::Hanami::ResponseMessage &hanamiResponse,
                               std::string &errorMessage)
{
    HanamiMessaging* messaging = HanamiMessaging::getInstance();

    // make token-request
    if(messaging->triggerSakuraFile(target,
                                    hanamiResponse,
                                    hanamiRequest,
                                    errorMessage) == false)
    {
        return internalError_ResponseBuild(m_httpResponse, errorMessage);
    }


    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          "authoriation failed!");
    }

    return success_ResponseBuild(m_httpResponse, hanamiResponse.responseContent);
}

/**
 * @brief parseUri
 * @param path
 * @param inputValues
 * @param uri
 * @return
 */
bool
HttpRequestEvent::parseUri(Kitsunemimi::Json::JsonItem &parsedInputValues,
                           std::string &target,
                           Kitsunemimi::Hanami::RequestMessage &request,
                           const std::string &uri,
                           std::string &errorMessage)
{
    // first split of uri
    std::vector<std::string> parts;
    Kitsunemimi::splitStringByDelimiter(parts, uri, '?');
    std::string key;
    std::string val;

    // check split-result
    if(parts.size() == 0) {
        return false;
    }
    if(parts.at(0).find("/") == std::string::npos) {
        return false;
    }

    if(parsedInputValues.parse(request.inputValues, errorMessage) == false) {
        return false;
    }

    // split first part again to get target and real part
    const size_t cutPos = parts.at(0).find("/");
    const std::string* mainPart = &parts[0];
    target = mainPart->substr(0, cutPos);
    request.id = mainPart->substr(cutPos + 1, mainPart->size() - 1);

    // prepare payload, if exist
    if(parts.size() > 1)
    {
        std::vector<std::string> kvPairs;
        Kitsunemimi::splitStringByDelimiter(kvPairs, parts[1], '&');

        for(const std::string &kvPair : kvPairs)
        {
            const size_t cutPos = kvPair.find('=');
            key = kvPair.substr(0, cutPos);
            val = kvPair.substr(cutPos + 1, kvPair.size() - 1);
            parsedInputValues.insert(key, val, true);
        }
    }

    request.inputValues = parsedInputValues.toString();

    return true;
}

/**
 * @brief HttpRequestEvent::checkPermission
 * @param token
 * @param component
 * @param endpoint
 * @param type
 * @return
 */
bool
HttpRequestEvent::checkPermission(const std::string &token,
                                  const std::string &component,
                                  const std::string &endpoint,
                                  const HttpRequestType type,
                                  Kitsunemimi::Hanami::ResponseMessage &responseMsg,
                                  std::string &errorMessage)
{
    Kitsunemimi::Hanami::RequestMessage requestMsg;

    requestMsg.id = "auth";
    requestMsg.inputValues = "{\"token\":\""
                             + token
                             + "\",\"component\":\""
                             + component
                             + "\",\"endpoint\":\""
                             + endpoint
                             + "\",\"http_type\":\""
                             + std::to_string(static_cast<uint32_t>(type))
                             + "\"}";
    requestMsg.httpType = HttpRequestType::GET_TYPE;

    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    return messaging->triggerSakuraFile("Misaka", responseMsg, requestMsg, errorMessage);
}

/**
 * @brief process control request by forwarding it to the backend
 *
 * @param path path to forward as identifier to trigger sakura file
 * @param inputValues json-formated input-values
 */
bool
HttpRequestEvent::processControlRequest(const std::string &uri,
                                        const std::string &inputValues,
                                        HttpRequestType httpType)
{
    std::string errorMessage = "";
    std::string target = "";
    Kitsunemimi::Hanami::RequestMessage hanamiRequest;
    Kitsunemimi::Hanami::ResponseMessage hanamiResponse;
    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    LOG_DEBUG("process uir: \'" + uri + "\'");

    // parse uri
    hanamiRequest.httpType = httpType;
    hanamiRequest.inputValues = inputValues;
    Kitsunemimi::Json::JsonItem parsedInputValues;
    if(parseUri(parsedInputValues, target, hanamiRequest, uri, errorMessage) == false) {
        return invalid_ResponseBuild(m_httpResponse, "failed to parse uri");
    }

    // handle token-request
    if(hanamiRequest.id == "token") {
        return requestToken(target, hanamiRequest, hanamiResponse, errorMessage);
    }

    // check authentication
    if(checkPermission(parsedInputValues.get("token").getString(),
                       target,
                       hanamiRequest.id,
                       hanamiRequest.httpType,
                       hanamiResponse,
                       errorMessage) == false)
    {
        return internalError_ResponseBuild(m_httpResponse, errorMessage);
    }

    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          "authoriation failed!");
    }

    // make real request
    if(messaging->triggerSakuraFile(target, hanamiResponse, hanamiRequest, errorMessage) == false) {
        return internalError_ResponseBuild(m_httpResponse, errorMessage);
    }

    // handle error-response
    if(hanamiResponse.type != 0)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    // handle success
    return success_ResponseBuild(m_httpResponse, hanamiResponse.responseContent);
}
