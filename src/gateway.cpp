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

#include <libKitsunemimiProjectNetwork/session.h>
#include <libKitsunemimiProjectNetwork/session_controller.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

namespace ToriiGateway
{

void streamDataCallback(void*,
                        Kitsunemimi::Project::Session*,
                        const void*,
                        const uint64_t)
{

}

/**
 * @brief standaloneDataCallback
 * @param target
 * @param data
 * @param dataSize
 */
void standaloneDataCallback(void*,
                            Kitsunemimi::Project::Session*,
                            const uint64_t,
                            Kitsunemimi::DataBuffer* data)
{
    delete data;
}

/**
 * @brief errorCallback
 */
void errorCallback(void*,
                   Kitsunemimi::Project::Session*,
                   const uint8_t,
                   const std::string)
{
    return;
}

/**
 * @brief sessionCallback
 * @param target
 * @param isInit
 * @param session
 * @param sessionIdentifier
 */
void sessionCallback(void* target,
                     bool isInit,
                     Kitsunemimi::Project::Session* incomingSession,
                     const std::string sessionIdentifier)
{
    Gateway* gateway = static_cast<Gateway*>(target);
    if(isInit)
    {
        if(incomingSession->isClientSide() == false)
        {
            LOG_INFO("register incoming session with session-identifier: " + sessionIdentifier);

            Kitsunemimi::Project::Session* outgoingSession = nullptr;

            bool success = false;
            std::string address = GET_STRING_CONFIG("Kyouko", "address", success);
            uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("Kyouko", "port", success));

            LOG_DEBUG("forward session to address: "
                      + address
                      + " and port: "
                      + std::to_string(port));

            outgoingSession = gateway->m_gatewayController->startTcpSession(address,
                                                                            port,
                                                                            sessionIdentifier);
            gateway->m_gatewayController->linkSessions(incomingSession, outgoingSession);
        }
    }
    else
    {
        // TODO: closing session
    }
}

/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway()
{
    m_gatewayController = new Kitsunemimi::Project::SessionController(this, &sessionCallback,
                                                                      this, &streamDataCallback,
                                                                      this, &standaloneDataCallback,
                                                                      this, &errorCallback);
}

/**
 * @brief Gateway::~Gateway
 */
Gateway::~Gateway()
{
    m_gatewayController->closeServer(m_serverId);
}

/**
 * @brief GatewayServer::initServer
 * @param port
 * @return
 */
bool Gateway::initServer()
{
    bool success = false;
    uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("Torii", "port", success));

    LOG_INFO("create gateway-server on port " + std::to_string(port));
    m_serverId = m_gatewayController->addTcpServer(port);
    return m_serverId != 0;
}

}
