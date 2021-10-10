/**
 * @file        gateway.cpp
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


#include "gateway.h"

#include <callbacks.h>
#include <http/request_queue.h>
#include <http/http_thread.h>

#include <libKitsunemimiCommon/logger.h>

#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>

#include <libKitsunemimiCommon/files/text_file.h>

using Kitsunemimi::Hanami::HanamiMessaging;

#include <websocket/web_socket_server.h>
#include <websocket/web_socket_session.h>
#include <http/http_server.h>

Gateway* Gateway::m_instance = new Gateway();
RequestQueue* Gateway::m_requestQueue = new RequestQueue();

/**
 * @brief constructor
 */
Gateway::Gateway() {}

/**
 * @brief initialize all server, which are set by the config-file
 *
 * @return true, if successful, else false
 */
bool
Gateway::initInternalSession()
{
    bool success = false;

    std::vector<std::string> groups = { "KyoukoMind" };
    success = HanamiMessaging::getInstance()->initialize("ToriiGateway", groups, false);
    if(success == false) {
        return false;
    }

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
Gateway::initWebSocketServer()
{
    bool success = false;

    // check if websocket is enabled
    if(GET_BOOL_CONFIG("server", "enable_websocket", success) == false) {
        return true;
    }

    // get stuff from config
    const long port = GET_INT_CONFIG("server", "websocket_port", success);
    const std::string ip = GET_STRING_CONFIG("server", "ip", success);

    // start websocket-server
    m_websocketServer = new WebSocketServer(ip, static_cast<uint16_t>(port));
    m_websocketServer->startThread();

    return true;
}

/**
 * @brief initialze http server
 *
 * @return true, if successful, else false
 */
bool
Gateway::initHttpServer()
{
    bool success = false;

    // get stuff from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("server", "http_port", success));
    const std::string ip = GET_STRING_CONFIG("server", "ip", success);
    const std::string cert = GET_STRING_CONFIG("server", "certificate", success);
    const std::string key = GET_STRING_CONFIG("server", "key", success);
    const uint32_t numberOfThreads = GET_INT_CONFIG("server", "number_of_threads", success);

    // start threads
    for(uint32_t i = 0; i < numberOfThreads; i++)
    {
        const std::string name = "HttpThread_" + std::to_string(i);
        HttpThread* httpThread = new HttpThread(name);
        httpThread->startThread();
        m_threads.push_back(httpThread);
    }

    m_httpServer = new HttpServer(ip, port, cert, key);
    m_httpServer->startThread();

    return true;
}
