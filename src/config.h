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

#include <libKitsunemimiConfig/config_handler.h>

/**
 * @brief register configs
 */
void
registerConfigs()
{
    // DEFAULT-section
    REGISTER_BOOL_CONFIG("DEFAULT", "debug", false);
    REGISTER_STRING_CONFIG("DEFAULT", "log_path", "/var/log");

    // server-section
    REGISTER_BOOL_CONFIG("server", "enable_websocket", false);
    REGISTER_BOOL_CONFIG("server", "enable_dashboard", false);
    REGISTER_STRING_CONFIG("server", "dashboard_files", "/etc/ToriiGateway/MikoClient");
    REGISTER_STRING_CONFIG("server", "ip", "0.0.0.0");
    REGISTER_STRING_CONFIG("server", "certificate", "");
    REGISTER_STRING_CONFIG("server", "key", "");
    REGISTER_INT_CONFIG("server", "http_port", 12345);
    REGISTER_INT_CONFIG("server", "websocket_port", 13345);
    REGISTER_INT_CONFIG("server", "number_of_threads", 4);

    // KyoukoMind-section
    REGISTER_STRING_CONFIG("KyoukoMind", "address", "/tmp/KyoukoMind_uds.sock");
    REGISTER_INT_CONFIG("KyoukoMind", "port", 0);
}

#endif // GATEWAY_CONFIG_H
