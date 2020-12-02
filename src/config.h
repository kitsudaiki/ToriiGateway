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

    // client-section
    REGISTER_BOOL_CONFIG("client", "enable", false);
    REGISTER_STRING_CONFIG("client", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("client", "http_port", 12345);
    REGISTER_INT_CONFIG("client", "websocket_port", 13345);
    REGISTER_STRING_CONFIG("client", "location", "/etc/ToriiGateway/MikoClient");

    // monitoring-section
    REGISTER_BOOL_CONFIG("monitoring", "enable", false);
    REGISTER_STRING_CONFIG("monitoring", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("monitoring", "http_port", 12346);
    REGISTER_INT_CONFIG("monitoring", "websocket_port", 13346);
    REGISTER_STRING_CONFIG("monitoring", "location", "/etc/ToriiGateway/MiyuMonitoring");

    // control-section
    REGISTER_BOOL_CONFIG("control", "enable", false);
    REGISTER_STRING_CONFIG("control", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("control", "http_port", 12347);

    // KyoukoMind-section
    REGISTER_STRING_CONFIG("KyoukoMind", "address", "/tmp/KyoukoMind_uds.sock");
    REGISTER_INT_CONFIG("KyoukoMind", "port", 0);
}

#endif // GATEWAY_CONFIG_H
