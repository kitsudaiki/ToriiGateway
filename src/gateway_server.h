/**
 *  @file    gateway_server.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#ifndef GATEWAY_SERVER_H
#define GATEWAY_SERVER_H

#include <stdint.h>

namespace Kitsune
{
namespace Network
{
class TcpServer;
class NetworkTrigger;
}
}

namespace ToriiGateway
{
class GatewayServerTrigger;

class GatewayServer
{
public:
    GatewayServer();

    bool initServer(const uint16_t port);

private:
    Kitsune::Network::TcpServer* m_server = nullptr;
    GatewayServerTrigger* m_trigger = nullptr;
    bool m_isInit = false;
};

}

#endif // GATEWAY_SERVER_H
