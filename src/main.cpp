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
#include <config.h>
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

    // init config-file
    std::string configPath = argParser.getStringValue("config");
    if(configPath == "") {
        configPath = "/etc/ToriiGateway/ToriiGateway.conf";
    }
    Kitsunemimi::Config::initConfig(configPath);

    // get config-parameter for logger
    bool success = false;
    const bool enableDebug = GET_BOOL_CONFIG("DEFAULT", "debug", success);
    assert(success);
    const std::string logPath = GET_STRING_CONFIG("DEFAULT", "log_path", success);
    assert(success);

    // init logger
    Kitsunemimi::Persistence::initConsoleLogger(enableDebug);
    Kitsunemimi::Persistence::initFileLogger(logPath, "ToriiGateway", enableDebug);

    // init gateway
    Gateway gateway;
    if(gateway.initClient() == false) {
        return 1;
    }
    if(gateway.initMonitoring() == false) {
        return 1;
    }
    if(gateway.initControl() == false) {
        return 1;
    }

    int a = 0;
    std::cin >> a;
    return 0;
}
