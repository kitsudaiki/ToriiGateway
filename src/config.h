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
    Kitsunemimi::Hanami::registerConfigs();

    // DEFAULT-section
    REGISTER_STRING_CONFIG("DEFAULT", "log_path", "/var/log");

    // server-section
    REGISTER_BOOL_CONFIG("server", "enable_websocket", false);
    REGISTER_BOOL_CONFIG("server", "enable_dashboard", false);
    REGISTER_STRING_CONFIG("server", "dashboard_files", "/etc/ToriiGateway/HanamiDashboard");
    REGISTER_STRING_CONFIG("server", "certificate", "");
    REGISTER_STRING_CONFIG("server", "key", "");
    REGISTER_STRING_CONFIG("server", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("server", "http_port", 12345);
    REGISTER_INT_CONFIG("server", "websocket_port", 13345);
    REGISTER_INT_CONFIG("server", "number_of_threads", 4);
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
        LOG_ERROR("port for websocket is not valie. Port in config: " + std::to_string(port));
        return false;
    }

    const std::string ip = GET_STRING_CONFIG("server", "ip", valid);


}

#endif // GATEWAY_CONFIG_H
