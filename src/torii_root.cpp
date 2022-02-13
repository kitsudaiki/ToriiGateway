/**
 * @file        torii_root.cpp
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

#include "torii_root.h"

#include <http/request_queue.h>
#include <http/http_thread.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>
#include <libKitsunemimiCommon/files/text_file.h>
#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>

using Kitsunemimi::Hanami::HanamiMessaging;

#include <websocket/web_socket_server.h>
#include <websocket/web_socket_session.h>
#include <http/http_server.h>
#include <api/blossom_initializing.h>

RequestQueue* ToriiGateway::requestQueue = new RequestQueue();

/**
 * @brief constructor
 */
ToriiGateway::ToriiGateway() {}

/**
 * @brief init torii-root-object
 *
 * @return true, if successfull, else false
 */
bool
ToriiGateway::init()
{
    Kitsunemimi::ErrorContainer error;

    if(initHttpServer() == false)
    {
        error.addMeesage("initializing http-server failed");
        LOG_ERROR(error);
        return false;
    }

    if(initWebSocketServer() == false)
    {
        error.addMeesage("initializing websocket-server failed");
        LOG_ERROR(error);
        return false;
    }

    if(initSakuraServer() == false)
    {
        error.addMeesage("initializing sakura-server failed");
        LOG_ERROR(error);
        return false;
    }

    initBlossoms();

    return true;
}

/**
 * @brief initialze websocket server
 *
 * @param group group-name in config-file
 *
 * @return true, if successful, else false
 */
bool
ToriiGateway::initWebSocketServer()
{
    bool success = false;

    // check if websocket is enabled
    if(GET_BOOL_CONFIG("websocket", "enable", success) == false) {
        return true;
    }

    // get stuff from config
    const long port = GET_INT_CONFIG("websocket", "port", success);
    const std::string ip = GET_STRING_CONFIG("websocket", "ip", success);

    // start websocket-server
    websocketServer = new WebSocketServer(ip, static_cast<uint16_t>(port));
    websocketServer->startThread();

    return true;
}

/**
 * @brief initialze http server
 *
 * @return true, if successful, else false
 */
bool
ToriiGateway::initHttpServer()
{
    bool success = false;

    // check if http is enabled
    if(GET_BOOL_CONFIG("http", "enable", success) == false) {
        return true;
    }

    // get stuff from config
    const uint16_t port =            GET_INT_CONFIG(    "http", "port",              success);
    const std::string ip =           GET_STRING_CONFIG( "http", "ip",                success);
    const std::string cert =         GET_STRING_CONFIG( "http", "certificate",       success);
    const std::string key =          GET_STRING_CONFIG( "http", "key",               success);
    const uint32_t numberOfThreads = GET_INT_CONFIG(    "http", "number_of_threads", success);

    // start threads
    for(uint32_t i = 0; i < numberOfThreads; i++)
    {
        const std::string name = "HttpThread";
        HttpThread* httpThread = new HttpThread(name);
        httpThread->startThread();
        m_threads.push_back(httpThread);
    }

    // create server
    httpServer = new HttpServer(ip, port, cert, key);
    httpServer->startThread();

    return true;
}

/**
 * @brief initialze sakura server
 *
 * @return true, if successful, else false
 */
bool
ToriiGateway::initSakuraServer()
{
    bool success = false;

    // check if sakura-server is enabled
    if(GET_BOOL_CONFIG("sakura", "enable", success) == false) {
        return true;
    }

    // get stuff from config
    const uint16_t port =    GET_INT_CONFIG(    "sakura", "port",        success);
    const std::string ip =   GET_STRING_CONFIG( "sakura", "ip",          success);
    const std::string cert = GET_STRING_CONFIG( "sakura", "certificate", success);
    const std::string key =  GET_STRING_CONFIG( "sakura", "key",         success);

    // create server
    Kitsunemimi::ErrorContainer error;
    HanamiMessaging* messaging = HanamiMessaging::getInstance();
    messaging->addServer(ip, error, port, cert, key);

    return true;
}