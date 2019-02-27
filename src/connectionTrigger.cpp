#include "connectionTrigger.h"

#include <communicationEnums.h>
#include <tcp/tcpClient.h>

namespace ToriiGateway
{

ConnectionTrigger::ConnectionTrigger(): Kitsune::Network::NetworkTrigger ()
{

}

ConnectionTrigger::~ConnectionTrigger()
{

}

void
ConnectionTrigger::runTask(uint8_t *buffer,
                           const long bufferSize,
                           Kitsune::Network::TcpClient *client)
{
    const uint8_t type = buffer[0];
    //const uint8_t source = buffer[1];
    const uint8_t target = buffer[2];

    if(target == MIND_TARGET && m_mindClient != nullptr)
    {
        m_mindClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == CLIENT_TARGET && m_clientClient != nullptr)
    {
        m_clientClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == MONITORING_TARGET && m_monitoringClient != nullptr)
    {
        m_monitoringClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == TORII_TARGET)
    {
        if(type == CLIENT_INIT_COM)
        {
            m_clientClient = client;
            return;
        }
        if(type == MONITORING_INIT_COM)
        {
            m_clientClient = client;
            return;
        }
        if(type == MIND_INIT_COM)
        {
            m_clientClient = client;
            return;
        }

        if(type == CLIENT_END_COM)
        {
            m_clientClient = nullptr;
            return;
        }
        if(type == MONITORING_END_COM)
        {
            m_clientClient = nullptr;
            return;
        }
        if(type == MIND_END_COM)
        {
            m_clientClient = nullptr;
            return;
        }
    }
}

}
