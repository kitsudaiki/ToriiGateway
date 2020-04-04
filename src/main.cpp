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
#include <config.h>

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

    // debugging for logging
    const bool enableDebug = argParser.wasSet("debug");
    Kitsunemimi::Persistence::initConsoleLogger(enableDebug);

    // config-file
    const std::string configFilePath = argParser.getStringValues("config").at(0);
    if(Kitsunemimi::Config::initConfig(configFilePath) == false) {
        return 1;
    }
    ToriiGateway::registerConfigs();

    // log-file
    std::string logDirPath = "/var/log/";
    if(argParser.wasSet("log-dir")) {
        logDirPath = argParser.getStringValues("log-dir").at(0);
    }
    std::string logBaseName = "ToriiGateway";
    if(argParser.wasSet("log-base")) {
        logBaseName = argParser.getStringValues("log-base").at(0);
    }
    Kitsunemimi::Persistence::initFileLogger(logDirPath, logBaseName, enableDebug);

    // create server
    ToriiGateway::Gateway gateway;
    gateway.initServer();

    int a = 0;
    std::cin >> a;
    return 0;
}
