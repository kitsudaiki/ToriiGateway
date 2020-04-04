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

#include <iostream>

namespace Kitsunemimi {
namespace Project {
class SessionController;
}
}

namespace ToriiGateway
{

class Gateway
{
public:
    Gateway();
    ~Gateway();

    bool initServer();

    Kitsunemimi::Project::SessionController* m_gatewayController = nullptr;

private:
    uint32_t m_serverId = 0;
};

}

#endif // GATEWAYSERVER_H
