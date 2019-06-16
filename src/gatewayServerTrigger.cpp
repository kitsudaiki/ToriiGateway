/**
 *  @file    gatewayServerTrigger.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gatewayServerTrigger.h"

#include <tcp/tcpClient.h>
#include <iostream>

namespace Kitsune
{
namespace Chan
{
namespace Communication
{

GatewayServerTrigger::GatewayServerTrigger() : Kitsune::Network::NetworkTrigger ()
{
}

GatewayServerTrigger::~GatewayServerTrigger()
{

}

uint32_t
GatewayServerTrigger::runTask(const MessageRingBuffer &recvBuffer,
                              Kitsune::Network::TcpClient *client)
{
    uint32_t processedBytes = 0;
    while(true)
    {
        // break-condition 1
        if(recvBuffer.readWriteDiff - processedBytes < 3) {
            break;
        }

        const uint8_t type = recvBuffer.data[(recvBuffer.readPosition + processedBytes) % recvBuffer.totalBufferSize];

        // get message-size
        const uint32_t messageSize = m_messageSize.sizes[type];
        if(type == UNDEFINED_TYPE) {
            std::cout<<"epic-fail"<<std::endl;
            break;
        }

        // break-condition 2
        if(recvBuffer.readWriteDiff - processedBytes < messageSize) {
            break;
        }

        const uint8_t source = recvBuffer.data[(recvBuffer.readPosition + 1 + processedBytes) % recvBuffer.totalBufferSize];
        const uint8_t target = recvBuffer.data[(recvBuffer.readPosition + 2 + processedBytes) % recvBuffer.totalBufferSize];

        // get data-block
        const uint8_t* dataPointer = getDataPointer(recvBuffer, m_tempBuffer, messageSize, processedBytes);
        processedBytes += messageSize;

        if(type == CLIENT_REGISTER_OUTPUT)
        {
            std::cout<<"CLIENT_REGISTER_OUTPUT"<<std::endl;
            const ClientRegisterOutput content = *((ClientRegisterOutput*)dataPointer);
            std::cout<<"poi: "<<content.brickId<<std::endl;
        }

        // normal data-messages
        if(target == MIND && m_mindClient != nullptr)
        {
            std::cout<<"to mind: "<<messageSize<<std::endl;
            if(m_mindClient->sendMessage(dataPointer, messageSize) == false) {
                std::cout<<"failed to send"<<std::endl;
            }
        }
        if(target == CLIENT && m_clientClient != nullptr)
        {
            std::cout<<"to client: "<<messageSize<<std::endl;
            if(m_clientClient->sendMessage(dataPointer, messageSize) == false) {
                std::cout<<"failed to send"<<std::endl;
            }
        }
        if(target == MONITORING && m_monitoringClient != nullptr)
        {
            std::cout<<"to monitoring: "<<messageSize<<std::endl;
            if(m_monitoringClient->sendMessage(dataPointer, messageSize) == false) {
                std::cout<<"failed to send"<<std::endl;
            }
        }

        // server-communication
        if(target == TORII)
        {
            if(type == SESSION_INIT)
            {
                if(source == CLIENT)
                {
                    std::cout<<"init client"<<std::endl;
                    if(m_clientClient != nullptr) {
                        m_clientClient->closeSocket();
                    }
                    m_clientClient = client;
                }
                if(source == MONITORING)
                {
                    std::cout<<"init monitoring"<<std::endl;
                    if(m_monitoringClient != nullptr) {
                        m_monitoringClient->closeSocket();
                    }
                    m_monitoringClient = client;
                }
                if(source == MIND)
                {
                    std::cout<<"init mind"<<std::endl;
                    if(m_mindClient != nullptr) {
                        m_mindClient->closeSocket();
                    }
                    m_mindClient = client;
                }
            }

            if(type == SESSION_END)
            {
                if(source == CLIENT && m_clientClient != nullptr)
                {
                    std::cout<<"end client"<<std::endl;
                    if(m_clientClient != nullptr) {
                        m_clientClient->closeSocket();
                    }
                    m_clientClient = nullptr;
                }
                if(source == MONITORING && m_monitoringClient != nullptr)
                {
                    std::cout<<"end monitoring"<<std::endl;
                    if(m_monitoringClient != nullptr) {
                        m_monitoringClient->closeSocket();
                    }
                    m_monitoringClient = nullptr;
                }
                if(source == MIND && m_mindClient != nullptr)
                {
                    std::cout<<"end mind"<<std::endl;
                    if(m_mindClient != nullptr) {
                        m_mindClient->closeSocket();
                    }
                    m_mindClient = nullptr;
                }

            }
        }
    }

    return processedBytes;
}

}
}
}
