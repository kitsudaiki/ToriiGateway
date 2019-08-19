/**
 *  @file    gatewayServerTrigger.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#ifndef CONNECTIONTRIGGER_H
#define CONNECTIONTRIGGER_H

#include <networkTrigger.h>
#include <vector>
#include <string>
#include <communicationStructs/communicationMessages.h>

namespace Kitsune
{
namespace Network
{
class TcpClient;
struct MessageRingBuffer;
}
}

using Kitsune::Network::MessageRingBuffer;

namespace Kitsune
{
namespace Chan
{
namespace Communication
{

class GatewayServerTrigger : public Kitsune::Network::NetworkTrigger
{
public:
    GatewayServerTrigger();
    ~GatewayServerTrigger();

    uint32_t runTask(const MessageRingBuffer& recvBuffer,
                     Kitsune::Network::TcpClient* client);

    uint8_t m_tempBuffer[8192];
    MessageSizes m_messageSize;

private:
    Kitsune::Network::TcpClient* m_mindClient = nullptr;
    Kitsune::Network::TcpClient* m_clientClient = nullptr;
    Kitsune::Network::TcpClient* m_monitoringClient = nullptr;
};

}
}
}

#endif // CONNECTIONTRIGGER_H
