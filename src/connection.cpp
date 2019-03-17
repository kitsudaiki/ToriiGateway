/**
 *  @file    connection.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "connection.h"

#include <gatewayServer.h>

namespace ToriiGateway
{

Connection::Connection()
{
    m_server = new GatewayServer();
    m_server->initServer(1337);
}

}
