/**
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 */

#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include <libKitsunemimiConfig/config_handler.h>

void
registerConfigs()
{
    REGISTER_BOOL_CONFIG("DEFAULT", "debug", false);
    REGISTER_STRING_CONFIG("DEFAULT", "log_path", "/var/log");

    REGISTER_BOOL_CONFIG("client", "enable", false);
    REGISTER_STRING_CONFIG("client", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("client", "http_port", 12345);
    REGISTER_INT_CONFIG("client", "websocket_port", 13345);

    REGISTER_BOOL_CONFIG("monitoring", "enable", false);
    REGISTER_STRING_CONFIG("monitoring", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("monitoring", "http_port", 12346);
    REGISTER_INT_CONFIG("monitoring", "websocket_port", 13346);

    REGISTER_BOOL_CONFIG("control", "enable", false);
    REGISTER_STRING_CONFIG("control", "ip", "0.0.0.0");
    REGISTER_INT_CONFIG("control", "http_port", 12347);

    REGISTER_STRING_CONFIG("KyoukoMind", "socket", "/tmp/KyoukoMind_uds.sock");
}

#endif // GATEWAY_CONFIG_H
