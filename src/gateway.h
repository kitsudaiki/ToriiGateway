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
namespace Kyouko {
class KyoukoNetwork;
}
}

namespace ToriiGateway
{

class Gateway
{
public:
    Gateway();

    bool initServer(const uint16_t port);

private:
    Kitsunemimi::Kyouko::KyoukoNetwork* m_kyoukoNetwork = nullptr;
};

}

#endif // GATEWAYSERVER_H
