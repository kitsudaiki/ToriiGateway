#include "connectionTrigger.h"

#include <communicationEnums.h>
#include <tcp/tcpClient.h>
#include <iostream>

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
    const uint8_t source = buffer[1];
    const uint8_t target = buffer[2];

    if(target == MIND && m_mindClient != nullptr)
    {
        m_mindClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == CLIENT && m_clientClient != nullptr)
    {
        m_clientClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == MONITORING && m_monitoringClient != nullptr)
    {
        m_monitoringClient->sendMessage(buffer, static_cast<uint32_t>(bufferSize));
    }

    if(target == TORII)
    {
        std::cout<<"poi"<<std::endl;
        if(type == SESSION_INIT)
        {
            if(source == CLIENT) {
                m_clientClient = client;
            }
            if(source == MONITORING) {
                m_monitoringClient = client;
            }
            if(source == MIND) {
                m_mindClient = client;
            }
            return;
        }

        if(type == SESSION_END)
        {
            if(source == CLIENT) {
                m_clientClient = nullptr;
            }
            if(source == MONITORING) {
                m_monitoringClient = nullptr;
            }
            if(source == MIND) {
                m_mindClient = nullptr;
            }
            return;
        }
    }
}

}
