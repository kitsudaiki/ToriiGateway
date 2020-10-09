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
#include <libKitsunemimiConfig/config_handler.h>

int main(int argc, char *argv[])
{
    Kitsunemimi::Persistence::initConsoleLogger(true);

    // create and init argument-parser
    Kitsunemimi::Args::ArgParser argParser;
    ToriiGateway::registerArguments(argParser);

    // parse cli-input
    if(argParser.parse(argc, argv) == false) {
        return 1;
    }

    // init logging
    const bool enableDebug = argParser.wasSet("debug");
    Kitsunemimi::Persistence::initConsoleLogger(enableDebug);
    Kitsunemimi::Persistence::initFileLogger("/var/log/",
                                             "ToriiGateway",
                                             enableDebug);

    // create server
    Gateway gateway;
    gateway.initHttpServer();

    int a = 0;
    std::cin >> a;
    return 0;
}
