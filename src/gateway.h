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

    bool initHttpServer();

};

#endif // GATEWAYSERVER_H
