/**
 *  @file    gatewayServer.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gateway.h"

#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

#include <libKitsunemimiPersistence/files/text_file.h>

using Kitsunemimi::Sakura::SakuraLangInterface;


#include <websocket/web_socket_server.h>
#include <http/http_server.h>
/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway() {}

/**
 * @brief Gateway::~Gateway
 */
Gateway::~Gateway() {}

/**
 * @brief Gateway::initClient
 * @return
 */
bool
Gateway::initClient()
{
    const std::string groupName = "client";
    if(isEnables(groupName) == false) {
        return false;
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
        return false;
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
        return false;
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

    WebSocketServer* server = new WebSocketServer(ip, port);
    server->startThread();

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

    HttpServer* httpServer = new HttpServer(ip, port);
    httpServer->startThread();

    return true;
}

