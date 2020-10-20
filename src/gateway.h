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

    bool initHttpServer(const std::string &address,
                        const uint16_t port);

private:
    const std::string getTestTree();
    const std::string getTestConfig();
};

#endif // GATEWAYSERVER_H
