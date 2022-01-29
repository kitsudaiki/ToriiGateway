/**
 * @file        config.h
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

#ifndef TORIIGATEWAY_GATEWAY_CONFIG_H
#define TORIIGATEWAY_GATEWAY_CONFIG_H

#include <filesystem>

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiHanamiCommon/config.h>

/**
 * @brief register configs
 */
void
registerConfigs(Kitsunemimi::ErrorContainer &error)
{
    Kitsunemimi::Hanami::registerBasicConfigs(error);

    // http-section
    const std::string httpGroup = "http";
    REGISTER_BOOL_CONFIG(   httpGroup, "enable",           error, false);
    REGISTER_BOOL_CONFIG(   httpGroup, "enable_dashboard", error, false);
    REGISTER_STRING_CONFIG( httpGroup, "dashboard_files",  error, "");
    REGISTER_STRING_CONFIG( httpGroup, "certificate",      error, "",        true);
    REGISTER_STRING_CONFIG( httpGroup, "key",              error, "",        true);
    REGISTER_STRING_CONFIG( httpGroup, "ip",               error, "0.0.0.0", true);
    REGISTER_INT_CONFIG(    httpGroup, "port",             error, 12345,     true);
    REGISTER_INT_CONFIG(    httpGroup, "number_of_threads", error, 4);

    // websocket-section
    const std::string websocketGroup = "websocket";
    REGISTER_BOOL_CONFIG(   websocketGroup, "enable",  error, false);
    REGISTER_INT_CONFIG(    websocketGroup, "port",    error, 13346);
    REGISTER_STRING_CONFIG( websocketGroup, "ip",               error, "0.0.0.0", true);

    // sakura-section
    const std::string sakuraGroup = "sakura";
    REGISTER_BOOL_CONFIG(   sakuraGroup, "enable",  error, false);
    REGISTER_STRING_CONFIG( sakuraGroup, "certificate",      error, "",        true);
    REGISTER_STRING_CONFIG( sakuraGroup, "key",              error, "",        true);
    REGISTER_STRING_CONFIG( sakuraGroup, "ip",               error, "0.0.0.0", true);
    REGISTER_INT_CONFIG(    sakuraGroup, "port",             error, 12345,     true);
}

bool
validateConfig(Kitsunemimi::ErrorContainer &error)
{
    bool valid = Kitsunemimi::Config::isConfigValid();
    if(valid == false) {
        return false;
    }

    const std::string fileLocation = GET_STRING_CONFIG("server", "dashboard_files", valid);
    if(valid == false) {
        return false;
    }

    if(std::filesystem::exists(fileLocation) == false) {
        return false;
    }

    const long port = GET_INT_CONFIG("server", "websocket_port", valid);
    if(port <= 0
            || port > 64000)
    {
        error.addMeesage("port for websocket is not valid. Port in config: "
                         + std::to_string(port));
        LOG_ERROR(error);
        return false;
    }

    const std::string ip = GET_STRING_CONFIG("server", "ip", valid);

    return true;
}

#endif // TORIIGATEWAY_GATEWAY_CONFIG_H
