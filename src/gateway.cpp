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

#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

#include <libKitsunemimiPersistence/files/text_file.h>

using Kitsunemimi::Sakura::SakuraLangInterface;
using Kitsunemimi::Sakura::MessagingController;
using Kitsunemimi::Sakura::MessagingClient;

#include <websocket/web_socket_server.h>
#include <websocket/web_socket_session.h>
#include <http/http_server.h>

Gateway* Gateway::m_instance = nullptr;
RequestQueue* Gateway::m_requestQueue = nullptr;

/**
 * @brief constructor
 */
Gateway::Gateway()
{
    m_instance = this;
    m_requestQueue = new RequestQueue();
    std::vector<std::string> groups = {};
    MessagingController::initializeMessagingController("ToriiGateway",
                                                       groups,
                                                       &messagingCreateCallback,
                                                       &messagingCloseCallback,
                                                       false);
}

/**
 * @brief destructor
 */
Gateway::~Gateway() {}

/**
 * @brief initialize all server, which are set by the config-file
 *
 * @return true, if successful, else false
 */
bool
Gateway::initInternalSession()
{
    MessagingClient* newClient = nullptr;
    bool success = false;

    const std::string address = GET_STRING_CONFIG("KyoukoMind", "address", success);
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("KyoukoMind", "port", success));

    MessagingController* contr = MessagingController::getInstance();
    newClient = contr->createClient("control", "control", address, port);
    if(newClient == nullptr) {
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

    // get port from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("server", "websocket_port", success));
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string ip = GET_STRING_CONFIG("server", "ip", success);
    if(success == false) {
        return false;
    }

    m_websocketServer = new WebSocketServer(ip, port, "server");
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

    // get port from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("server", "http_port", success));
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string ip = GET_STRING_CONFIG("server", "ip", success);
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string cert = GET_STRING_CONFIG("server", "certificate", success);
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string key = GET_STRING_CONFIG("server", "key", success);
    if(success == false) {
        return false;
    }

    const uint32_t numberOfThreads = GET_INT_CONFIG("server", "number_of_threads", success);
    if(success == false) {
        return false;
    }

    for(uint32_t i = 0; i < numberOfThreads; i++)
    {
        HttpThread* httpThread = new HttpThread();
        httpThread->startThread();
        m_threads.push_back(httpThread);
    }

    m_httpServer = new HttpServer(ip, port, cert, key);
    m_httpServer->startThread();

    return true;
}

/**
 * @brief create and add new client to internal list
 *
 * @param id name of the client for later identification
 * @param session session for the new client
 *
 * @return new client, or nullptr, if client-name already exist
 */
bool
Gateway::addClient(const std::string &id,
                   Kitsunemimi::Sakura::MessagingClient* session)
{
    // check if already used
    std::map<std::string, MessagingClient*>::const_iterator it;
    it = m_clients.find(id);
    if(it != m_clients.end()) {
        return false;
    }

    m_clients.insert(std::make_pair(id, session));

    return true;
}

/**
 * @brief remove client from list
 *
 * @param id id of the requested client
 *
 * @return true, if successful, else false
 */
bool
Gateway::removeClient(const std::string &id)
{
    std::map<std::string, MessagingClient*>::const_iterator it;
    it = m_clients.find(id);
    if(it != m_clients.end())
    {
        m_clients.erase(it);
        return true;
    }

    return false;
}

/**
 * @brief get client from the internal list
 *
 * @param id id of the requested client
 *
 * @return pointer to client, if found, else nullptr
 */
MessagingClient*
Gateway::getClient(const std::string &id)
{
    std::map<std::string, MessagingClient*>::const_iterator it;
    it = m_clients.find(id);
    if(it != m_clients.end()) {
        return it->second;
    }

    return nullptr;
}
