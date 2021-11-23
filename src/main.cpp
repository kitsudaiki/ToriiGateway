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
    Kitsunemimi::ErrorContainer error;
    if(initMain(argc, argv, "torii", &registerArguments, &registerConfigs, error) == false) {
        return 1;
    }

    // init gateway
    Gateway gateway;
    if(gateway.initHttpServer() == false)
    {
        error.addMeesage("initializing http-server failed");
        LOG_ERROR(error);
        return 1;
    }
    if(gateway.initWebSocketServer() == false)
    {
        error.addMeesage("initializing websocket-server failed");
        LOG_ERROR(error);
        return 1;
    }
    if(gateway.initInternalSession(error) == false)
    {
        error.addMeesage("initializing connection to backend failed");
        LOG_ERROR(error);
        return 1;
    }

    // sleep forever
    std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());

    return 0;
}
