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

    bool initInternalSession();
    bool initClient();
    bool initMonitoring();
    bool initControl();

private:
    bool isEnables(const std::string &group);
    bool initWebSocketServer(const std::string &group);
    bool initHttpServer(const std::string &group);

};

#endif // GATEWAYSERVER_H
