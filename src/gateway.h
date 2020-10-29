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

class Gateway
{
public:
    Gateway();
    ~Gateway();

    bool initWebSocketServer(const std::string &address,
                             const uint16_t port);
    bool initHttpServer(const std::string &address,
                        const uint16_t port);

};

#endif // GATEWAYSERVER_H
