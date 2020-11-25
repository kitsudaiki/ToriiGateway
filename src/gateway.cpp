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

/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway()
{
    std::vector<std::string> groups = {};
    MessagingController::initializeMessagingController("ToriiGateway",
                                                       groups,
                                                       this,
                                                       &sessionCallback,
                                                       false);
}

/**
 * @brief Gateway::~Gateway
 */
Gateway::~Gateway() {}

/**
 * @brief Gateway::initInternalSession
 * @return
 */
bool
Gateway::initInternalSession()
{
    MessagingClient* newClient = nullptr;
    bool success = false;

    const std::string address = GET_STRING_CONFIG("KyoukoMind", "address", success);
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("KyoukoMind", "port", success));

    const std::string clients[3] = {"control", "client", "monitoring"};
    for(const std::string &clientName : clients)
    {
        newClient = MessagingController::getInstance()->createClient(clientName,
                                                                     clientName,
                                                                     address,
                                                                     port);
        if(newClient == nullptr) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Gateway::initClient
 * @return
 */
bool
Gateway::initClient()
{
    const std::string groupName = "client";
    if(isEnables(groupName) == false) {
        return true;
    }

    if(initHttpServer(groupName) == false) {
        return false;
    }

    if(initWebSocketServer(groupName) == false) {
        return false;
    }

    return true;
}

/**
 * @brief Gateway::initMonitoring
 * @return
 */
bool
Gateway::initMonitoring()
{
    const std::string groupName = "monitoring";
    if(isEnables(groupName) == false) {
        return true;
    }

    if(initHttpServer(groupName) == false) {
        return false;
    }

    if(initWebSocketServer(groupName) == false) {
        return false;
    }

    return true;
}

/**
 * @brief Gateway::initControl
 * @return
 */
bool
Gateway::initControl()
{
    const std::string groupName = "control";
    if(isEnables(groupName) == false) {
        return true;
    }

    return initHttpServer(groupName);
}

/**
 * @brief Gateway::isEnables
 * @param group
 * @return
 */
bool
Gateway::isEnables(const std::string &group)
{
    bool success = false;
    if(GET_BOOL_CONFIG(group, "enable", success)) {
        return true;
    }

    return false;
}

/**
 * @brief Gateway::initWebSocketServer
 * @param address
 * @param port
 * @return
 */
bool
Gateway::initWebSocketServer(const std::string &group)
{
    bool success = false;

    // get port from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG(group, "websocket_port", success));
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string ip = GET_STRING_CONFIG(group, "ip", success);
    if(success == false) {
        return false;
    }

    m_websocketServer = new WebSocketServer(ip, port, group);
    m_websocketServer->startThread();

    return true;
}

/**
 * @brief Gateway::initHttpServer
 * @param address
 * @param port
 * @return
 */
bool
Gateway::initHttpServer(const std::string &group)
{
    bool success = false;

    // get port from config
    const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG(group, "http_port", success));
    if(success == false) {
        return false;
    }

    // get ip to bind from config
    const std::string ip = GET_STRING_CONFIG(group, "ip", success);
    if(success == false) {
        return false;
    }

    m_httpServer = new HttpServer(ip, port, group);
    m_httpServer->startThread();

    return true;
}

