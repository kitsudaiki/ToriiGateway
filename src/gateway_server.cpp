/**
 *  @file    gateway_server.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gateway_server.h"
#include <tcp/tcp_server.h>
#include <gateway_server_trigger.h>

namespace ToriiGateway
{

/**
 * @brief GatewayServer::GatewayServer
 */
GatewayServer::GatewayServer()
{
    m_trigger = new GatewayServerTrigger();
}

/**
 * @brief GatewayServer::initServer
 * @param port
 * @return
 */
bool GatewayServer::initServer(const uint16_t port)
{
    if(m_isInit == true) {
        return true;
    }

    m_server = new Kitsune::Network::TcpServer(m_trigger);

    bool result = m_server->initSocket(port);

    if(result)
    {
        m_server->start();
        m_isInit = true;
    }

    return result;
}

}
