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

#include <websocket/web_socket_server.h>
#include <http/http_server.h>
/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway()
{

}

/**
 * @brief Gateway::~Gateway
 */
Gateway::~Gateway()
{
}

/**
 * @brief Gateway::initWebSocketServer
 * @param address
 * @param port
 * @return
 */
bool
Gateway::initWebSocketServer(const std::string &address,
                             const uint16_t port)
{
    WebSocketServer* server = new WebSocketServer(address, port);
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
Gateway::initHttpServer(const std::string &address,
                        const uint16_t port)
{
    HttpServer* httpServer = new HttpServer(address, port);
    httpServer->startThread();

    return true;
}

