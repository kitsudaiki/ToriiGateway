/**
 *  @file    gatewayServer.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#ifndef GATEWAYSERVER_H
#define GATEWAYSERVER_H

#include <stdint.h>

namespace Kitsune
{
namespace Network
{
class TcpServer;
class NetworkTrigger;
}

namespace Chan
{
namespace Communication
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
}
}

#endif // GATEWAYSERVER_H
