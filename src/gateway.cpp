/**
 *  @file    gatewayServer.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gateway.h"
#include <callbacks.h>

#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiKyoukoNetwork/kyouko_network.h>
#include <libKitsunemimiKyoukoNetwork/sessions/kyouko_session.h>

namespace ToriiGateway
{


void sessionCallback(void* gateway,
                     bool isInit,
                     Kitsunemimi::Kyouko::KyoukoSession* newSession)
{
    LOG_INFO("register incoming new session");
}

/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway()
{
    m_kyoukoNetwork = new Kitsunemimi::Kyouko::KyoukoNetwork(this, &sessionCallback);
}

/**
 * @brief GatewayServer::initServer
 * @param port
 * @return
 */
bool Gateway::initServer(const uint16_t port)
{
    LOG_INFO("create gateway-server on port " + std::to_string(port));
    return m_kyoukoNetwork->createTcpServer(port);
}

}
