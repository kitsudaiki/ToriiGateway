/**
 *  @file    main.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include <iostream>
#include <gatewayServer.h>

using Kitsune::Chan::Communication::GatewayServer;

int main()
{
    GatewayServer* m_server = new GatewayServer();
    m_server->initServer(1337);

    int a = 0;
    std::cin >> a;
    return 0;
}
