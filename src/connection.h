/**
 *  @file    connection.h
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#ifndef CONNECTION_H
#define CONNECTION_H

namespace Kitsune {
namespace Chan {
namespace Common {
class GatewayServer;
}
}
}

using Kitsune::Chan::Common::GatewayServer;

namespace ToriiGateway
{
class ConnectionTrigger;

class Connection
{
public:
    Connection();

private:
    GatewayServer* m_server = nullptr;
};

}

#endif // CONNECTION_H
