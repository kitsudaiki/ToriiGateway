/**
 * @file        http_processing.cpp
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

#include "http_processing.h"

#include <torii_root.h>
#include <core/http_processing/file_send.h>
#include <core/http_processing/response_builds.h>
#include <core/http_processing/string_functions.h>
#include <core/http_server.h>

#include <libKitsunemimiJson/json_item.h>
#include <libKitsunemimiCommon/logger.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>
#include <libKitsunemimiHanamiMessaging/hanami_messaging_client.h>

#include <libSagiriArchive/sagiri_send.h>

using Kitsunemimi::Hanami::HanamiMessaging;
using Kitsunemimi::Hanami::HanamiMessagingClient;
using Kitsunemimi::Sakura::SakuraLangInterface;

/**
 * @brief process request and build response
 */
void
processRequest(http::request<http::string_body> &httpRequest,
               http::response<http::dynamic_body> &httpResponse)
{
    // build default-header for response
    httpResponse.version(httpRequest.version());
    httpResponse.keep_alive(false);
    httpResponse.set(http::field::server, "ToriiGateway");
    httpResponse.result(http::status::ok);
    httpResponse.set(http::field::content_type, "text/plain");
    Kitsunemimi::ErrorContainer error;

    // collect and prepare relevant data
    const http::verb messageType = httpRequest.method();
    std::string path = httpRequest.target().to_string();
    std::string payload = "{}";

    // Request path must be absolute and not contain "..".
    if(checkPath(path) == false)
    {
        httpResponse.result(http::status::bad_request);
        return;
    }

    // check if http-type is supported
    if(messageType != http::verb::get
            && messageType != http::verb::post
            && messageType != http::verb::put
            && messageType != http::verb::delete_)
    {
        httpResponse.result(http::status::bad_request);
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("Invalid request-method '"
                         + std::string(httpRequest.method_string())
                         + "'");
        LOG_ERROR(error);
        beast::ostream(httpResponse.body()) << error.toString();
        return;
    }

    // check for dashboard-client-request
    if(messageType == http::verb::get
            && cutPath(path, "/client"))
    {
        if(processClientRequest(httpResponse, path, error) == false) {
            LOG_ERROR(error);
        }
        return;
    }

    // get payload of message
    if(messageType == http::verb::post
            || messageType == http::verb::put)
    {
        payload = httpRequest.body().data();
    }

    // get token from request-header
    std::string token = "";
    if(httpRequest.count("X-Auth-Token") > 0) {
        token = httpRequest.at("X-Auth-Token").to_string();
    }

    // handle control-messages
    if(cutPath(path, "/control/"))
    {
        HttpRequestType hType = static_cast<HttpRequestType>(messageType);
        if(processControlRequest(httpResponse, path, token, payload, hType, error) == false) {
            LOG_ERROR(error);
        }
        return;
    }

    // handle default, if nothing was found
    error.addMeesage("no matching endpoint found");
    genericError_ResponseBuild(httpResponse,
                               HttpResponseTypes::NOT_FOUND_RTYPE,
                               error.toString());
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
requestToken(http::response<http::dynamic_body> &httpResponse,
             const std::string &target,
             const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
             Kitsunemimi::Hanami::ResponseMessage &hanamiResponse,
             Kitsunemimi::ErrorContainer &error)
{
    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    HanamiMessagingClient* client = messaging->getOutgoingClient(target);
    if(client == nullptr)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          "Client '" + target + "' not found");
    }

    // make token-request
    if(client->triggerSakuraFile(hanamiResponse, hanamiRequest, error) == false)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }


    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE
            || hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    return success_ResponseBuild(httpResponse, hanamiResponse.responseContent);
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
checkPermission(const std::string &token,
                const std::string &component,
                const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                Kitsunemimi::Hanami::ResponseMessage &responseMsg,
                Kitsunemimi::ErrorContainer &error)
{
    Kitsunemimi::Hanami::RequestMessage requestMsg;

    requestMsg.id = "v1/auth";
    requestMsg.httpType = HttpRequestType::GET_TYPE;
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

    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    if(messaging->misakaClient == nullptr)
    {
        // TODO: handle error
        return false;
    }
    return messaging->misakaClient->triggerSakuraFile(responseMsg, requestMsg, error);
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
processControlRequest(http::response<http::dynamic_body> &httpResponse,
                      const std::string &uri,
                      const std::string &token,
                      const std::string &inputValues,
                      const HttpRequestType httpType,
                      Kitsunemimi::ErrorContainer &error)
{
    std::string target = "";
    Kitsunemimi::Hanami::RequestMessage hanamiRequest;
    Kitsunemimi::Hanami::ResponseMessage hanamiResponse;
    HanamiMessaging* messaging = HanamiMessaging::getInstance();

    // parse uri
    hanamiRequest.httpType = httpType;
    hanamiRequest.inputValues = inputValues;
    if(parseUri(target, token, hanamiRequest, uri, error) == false) {
        return invalid_ResponseBuild(httpResponse, "failed to parse uri");
    }

    // handle token-request
    if(hanamiRequest.id == "v1/token") {
        return requestToken(httpResponse, target, hanamiRequest, hanamiResponse, error);
    }

    // check authentication
    if(checkPermission(token, target, hanamiRequest, hanamiResponse, error) == false) {
        return internalError_ResponseBuild(httpResponse, error);
    }

    // handle failed authentication
    if(hanamiResponse.type == Kitsunemimi::Hanami::UNAUTHORIZED_RTYPE
            || hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    // parse response to get user-uuid of the token
    Kitsunemimi::Json::JsonItem userData;
    if(userData.parse(hanamiResponse.responseContent, error) == false) {
        return internalError_ResponseBuild(httpResponse, error);
    }

    // send audit-message to sagiri
    Sagiri::sendAuditMessage(target,
                             hanamiRequest.id,
                             userData.get("uuid").getString(),
                             hanamiRequest.httpType);

    // forward real request
    HanamiMessagingClient* client = messaging->getOutgoingClient(target);
    if(client == nullptr)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          "Client '" + target + "' not found");
    }

    // make real request
    if(client->triggerSakuraFile(hanamiResponse, hanamiRequest, error) == false) {
        return internalError_ResponseBuild(httpResponse, error);
    }

    // handle error-response
    if(hanamiResponse.success == false)
    {
        return genericError_ResponseBuild(httpResponse,
                                          hanamiResponse.type,
                                          hanamiResponse.responseContent);
    }

    // handle success
    return success_ResponseBuild(httpResponse, hanamiResponse.responseContent);
}
