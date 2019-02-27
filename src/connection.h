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

namespace Kitsune
{
namespace Network
{
class TcpServer;
class TcpClient;
}
}

namespace ToriiGateway
{

class Connection
{
public:
    Connection();
private:
    Kitsune::Network::TcpClient* m_client = nullptr;
    Kitsune::Network::TcpServer* m_server = nullptr;
};

}

#endif // CONNECTION_H
