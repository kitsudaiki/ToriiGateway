/**
 * @file        main.cpp
 *
 * @author      Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright   Apache License Version 2.0
 *
 *      Copyright 2019 Tobias Anker
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
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
    registerConfigs();

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
    if(gateway.initClient() == false)
    {
        LOG_ERROR("initializing client-endpoint failed");
        return 1;
    }
    if(gateway.initMonitoring() == false)
    {
        LOG_ERROR("initializing monitoring-endpoint failed");
        return 1;
    }
    if(gateway.initHttpServer() == false)
    {
        LOG_ERROR("initializing http-server failed");
        return 1;
    }
    if(gateway.initInternalSession() == false)
    {
        LOG_ERROR("initializing connection to backend failed");
        return 1;
    }

    int a = 0;
    std::cin >> a;
    return 0;
}
