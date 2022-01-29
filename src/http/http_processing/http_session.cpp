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
#include <torii_root.h>
#include <http/http_processing/file_send.h>
#include <http/http_processing/response_builds.h>
#include <http/http_processing/string_functions.h>

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiJson/json_item.h>

#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiCommon/common_methods/string_methods.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>

using Kitsunemimi::Hanami::HanamiMessaging;
using Kitsunemimi::Sakura::SakuraLangInterface;

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
        error.addMeesage("SSL-Handshake failed!");
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
    Kitsunemimi::ErrorContainer error;

    // collect and prepare relevant data
    const http::verb messageType = m_httpRequest.method();
    std::string path = m_httpRequest.target().to_string();
    std::string payload = "{}";

    // Request path must be absolute and not contain "..".
    if(checkPath(path) == false)
    {
        m_httpResponse.result(http::status::bad_request);
        return;
    }

    // check if http-type is supported
    if(messageType != http::verb::get
            && messageType != http::verb::post
            && messageType != http::verb::put
            && messageType != http::verb::delete_)
    {
        m_httpResponse.result(http::status::bad_request);
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("Invalid request-method '"
                         + std::string(m_httpRequest.method_string())
                         + "'");
        LOG_ERROR(error);
        beast::ostream(m_httpResponse.body()) << error.toString();
        return;
    }

    // check for dashboard-client-request
    if(messageType == http::verb::get
            && cutPath(path, "/client"))
    {
        if(processClientRequest(m_httpResponse, path, error) == false) {
            LOG_ERROR(error);
        }
        return;
    }

    // get payload of message
    if(messageType == http::verb::post
            || messageType == http::verb::put)
    {
        payload = m_httpRequest.body().data();
    }

    // get token from request-header
    std::string token = "";
    if(m_httpRequest.count("X-Auth-Token") > 0) {
        token = m_httpRequest.at("X-Auth-Token").to_string();
    }

    // handle control-messages
    if(cutPath(path, "/control/"))
    {
        HttpRequestType hType = static_cast<HttpRequestType>(messageType);
        if(processControlRequest(path, token, payload, hType, error) == false) {
            LOG_ERROR(error);
        }
        return;
    }

    // handle default, if nothing was found
    error.addMeesage("no matching endpoint found");
    genericError_ResponseBuild(m_httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               error.toString());
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
    std::string result = "";
    result.append("{\"port\":");
    result.append(std::to_string(port));
    result.append(",\"ip\":\"");
    result.append(ip);
    result.append("\"}");

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
        error.addMeesage("read: " + ec.message());
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
        error.addMeesage("write: " + ec.message());
        LOG_ERROR(error);
        return false;
    }

    return true;
}

/**
 * @brief request token from misaka
 *
 * @param target target (misaka)
 * @param hanamiRequest hanami-request for the token-request
 * @param hanamiResponse reference for the response
 * @param error reference for error-output
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::requestToken(const std::string &target,
                               const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                               Kitsunemimi::Hanami::ResponseMessage &hanamiResponse,
                               Kitsunemimi::ErrorContainer &error)
{
    HanamiMessaging* messaging = HanamiMessaging::getInstance();

    // make token-request
    if(messaging->triggerSakuraFile(target, hanamiResponse, hanamiRequest, error) == false)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }


    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE
            || hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }


    return success_ResponseBuild(m_httpResponse, hanamiResponse.responseContent);
}

/**
 * @brief send request to misaka to check permissions
 *
 * @param token token to validate
 * @param component requested compoent
 * @param hanamiRequest hanami-request to the requested endpoint
 * @param responseMsg reference for the response
 * @param error reference for error-output
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::checkPermission(const std::string &token,
                                  const std::string &component,
                                  const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                                  Kitsunemimi::Hanami::ResponseMessage &responseMsg,
                                  Kitsunemimi::ErrorContainer &error)
{
    Kitsunemimi::Hanami::RequestMessage requestMsg;

    requestMsg.id = "v1/auth";
    requestMsg.inputValues = "";
    requestMsg.inputValues.append("{\"token\":\"");
    requestMsg.inputValues.append(token);
    requestMsg.inputValues.append("\",\"component\":\"");
    requestMsg.inputValues.append(component);
    requestMsg.inputValues.append("\",\"endpoint\":\"");
    requestMsg.inputValues.append(hanamiRequest.id);
    requestMsg.inputValues.append("\",\"http_type\":");
    requestMsg.inputValues.append(std::to_string(static_cast<uint32_t>(hanamiRequest.httpType)));
    requestMsg.inputValues.append("}");

    requestMsg.httpType = HttpRequestType::GET_TYPE;

    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    return messaging->triggerSakuraFile("misaka", responseMsg, requestMsg, error);
}

/**
 * @brief handle api-request within the torii
 *
 * @param hanamiRequest request-data
 * @param responseMsg reference for the response-message
 * @param error reference for error-ourpur
 */
void
HttpRequestEvent::internalRequest(const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                                  Kitsunemimi::Hanami::ResponseMessage &responseMsg,
                                  Kitsunemimi::ErrorContainer &error)
{
    SakuraLangInterface* interface = SakuraLangInterface::getInstance();
    Kitsunemimi::DataMap result;
    Kitsunemimi::DataMap context;
    Kitsunemimi::Sakura::BlossomStatus status;

    // parse json-formated input values
    Kitsunemimi::Json::JsonItem inputValues;
    if(inputValues.parse(hanamiRequest.inputValues, error) == false)
    {
        responseMsg.success = false;
        responseMsg.type = Kitsunemimi::Hanami::BAD_REQUEST_RTYPE;
        return;
    }

    // remove token from request, with would be normally done by the messaging-lib
    Kitsunemimi::DataMap* inputs = inputValues.getItemContent()->toMap();
    inputs->remove("token");

    // trigger blossom within the torii
    if(interface->triggerBlossom(result,
                                 hanamiRequest.id,
                                 "proxy",
                                 context,
                                 *inputs,
                                 status,
                                 error) == false)
    {
        responseMsg.success = false;
        responseMsg.type = static_cast<HttpResponseTypes>(status.statusCode);
        responseMsg.responseContent = status.errorMessage;
        return;
    }

    responseMsg.success = true;

    return;
}

/**
 * @brief process control request
 *
 * @param uri requested uri
 * @param token given token coming from the http-header
 * @param inputValues json-formated input-values
 * @param httpType type of the http-request
 * @param error reference for error-output
 *
 * @return true, if successful, else false
 */
bool
HttpRequestEvent::processControlRequest(const std::string &uri,
                                        const std::string &token,
                                        const std::string &inputValues,
                                        const HttpRequestType httpType,
                                        Kitsunemimi::ErrorContainer &error)
{
    std::string target = "";
    Kitsunemimi::Hanami::RequestMessage hanamiRequest;
    Kitsunemimi::Hanami::ResponseMessage hanamiResponse;
    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    std::string stringType = "";
    if(httpType == HttpRequestType::GET_TYPE) {
        stringType = "get";
    }
    if(httpType == HttpRequestType::POST_TYPE) {
        stringType = "post";
    }
    if(httpType == HttpRequestType::PUT_TYPE) {
        stringType = "put";
    }
    if(httpType == HttpRequestType::DELETE_TYPE) {
        stringType = "delete";
    }
    LOG_DEBUG("process uri: \'" + uri + "\' with type '" + stringType + "'");

    // parse uri
    hanamiRequest.httpType = httpType;
    hanamiRequest.inputValues = inputValues;
    if(parseUri(target, token, hanamiRequest, uri, error) == false) {
        return invalid_ResponseBuild(m_httpResponse, "failed to parse uri");
    }

    // handle token-request
    if(hanamiRequest.id == "v1/token") {
        return requestToken(target, hanamiRequest, hanamiResponse, error);
    }

    // check authentication
    if(checkPermission(token, target, hanamiRequest, hanamiResponse, error) == false) {
        return internalError_ResponseBuild(m_httpResponse, error);
    }

    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE
            || hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    if(target == "torii")
    {
        internalRequest(hanamiRequest, hanamiResponse, error);
    }
    else
    {
        // make real request
        if(messaging->triggerSakuraFile(target, hanamiResponse, hanamiRequest, error) == false) {
            return internalError_ResponseBuild(m_httpResponse, error);
        }
    }

    // handle error-response
    if(hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(m_httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    // handle success
    return success_ResponseBuild(m_httpResponse, hanamiResponse.responseContent);
}
