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

#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include <filesystem>

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiHanamiCommon/config.h>

/**
 * @brief register configs
 */
void
registerConfigs()
{
    Kitsunemimi::Hanami::registerBasicConfigs();

    // server-section
    const std::string serverGroup = "server";
    REGISTER_BOOL_CONFIG(   serverGroup, "enable_websocket",  false);
    REGISTER_BOOL_CONFIG(   serverGroup, "enable_dashboard",  false);
    REGISTER_STRING_CONFIG( serverGroup, "dashboard_files",   "");
    REGISTER_STRING_CONFIG( serverGroup, "certificate",       "",        true);
    REGISTER_STRING_CONFIG( serverGroup, "key",               "",        true);
    REGISTER_STRING_CONFIG( serverGroup, "ip",                "0.0.0.0", true);
    REGISTER_INT_CONFIG(    serverGroup, "http_port",         12345,     true);
    REGISTER_INT_CONFIG(    serverGroup, "websocket_port",    13345);
    REGISTER_INT_CONFIG(    serverGroup, "number_of_threads", 4);
}

bool
validateConfig()
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
        Kitsunemimi::ErrorContainer error;
        error.errorMessage = "port for websocket is not valid. Port in config: "
                             + std::to_string(port);
        LOG_ERROR(error);
        return false;
    }

    const std::string ip = GET_STRING_CONFIG("server", "ip", valid);

    return true;
}

#endif // GATEWAY_CONFIG_H
