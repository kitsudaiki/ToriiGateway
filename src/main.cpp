/**
 *  @file    main.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include <iostream>

#include <gateway.h>
#include <args.h>

#include <libKitsunemimiArgs/arg_parser.h>
#include <libKitsunemimiPersistence/logger/logger.h>

int main(int argc, char *argv[])
{
    Kitsunemimi::Persistence::initConsoleLogger(false);

    Kitsunemimi::Args::ArgParser argParser;
    ToriiGateway::registerArguments(argParser);

    if(argParser.parse(argc, argv) == false) {
        return 1;
    }

    if(argParser.wasSet("debug")) {
        Kitsunemimi::Persistence::initConsoleLogger(true);
    }

    std::vector<long> ports = argParser.getIntValues("port");
    uint16_t port = static_cast<uint16_t>(ports.at(0));

    ToriiGateway::Gateway gateway;
    gateway.initServer(port);

    int a = 0;
    std::cin >> a;
    return 0;
}
