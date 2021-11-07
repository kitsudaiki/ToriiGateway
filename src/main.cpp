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
#include <thread>

#include <libKitsunemimiArgs/arg_parser.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiHanamiCommon/generic_main.h>

using Kitsunemimi::Hanami::initMain;

int main(int argc, char *argv[])
{
    if(initMain(argc, argv, "ToriiGateway", &registerArguments, &registerConfigs) == false) {
        return 1;
    }

    // init gateway
    Kitsunemimi::ErrorContainer error;
    if(Gateway::gateway->initHttpServer() == false)
    {
        error.errorMessage = "initializing http-server failed";
        LOG_ERROR(error);
        return 1;
    }
    if(Gateway::gateway->initWebSocketServer() == false)
    {
        error.errorMessage = "initializing websocket-server failed";
        LOG_ERROR(error);
        return 1;
    }
    if(Gateway::gateway->initInternalSession() == false)
    {
        error.errorMessage = "initializing connection to backend failed";
        LOG_ERROR(error);
        return 1;
    }

    // sleep forever
    std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());

    return 0;
}
