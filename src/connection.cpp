/**
 *  @file    connection.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "connection.h"

#include <tcp/tcpServer.h>
#include <tcp/tcpClient.h>

namespace ToriiGateway
{

Connection::Connection()
{
    m_server = new Kitsune::Network::TcpServer();

    // TODO: configurable port-number
    m_server->initSocket(1337);
    m_server->start();
}

}
