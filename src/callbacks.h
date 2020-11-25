/**
 * @file       callbacks.h
 *
 * @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright  Apache License Version 2.0
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

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gateway.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

#include <websocket/web_socket_server.h>
#include <websocket/web_socket_session.h>
#include <http/http_server.h>

void clientDataCallback(void* target,
                        Kitsunemimi::Sakura::Session*,
                        const void* data,
                        const uint64_t dataSize)
{
    Gateway* gateway = static_cast<Gateway*>(target);
    WebSocketServer* server = gateway->m_websocketServer;
    const std::string text(static_cast<const char*>(data), dataSize);

    for(uint64_t i = 0; i < server->m_activeClientSessions.size(); i++) {
        server->m_activeClientSessions.at(i)->sendText(text);
    }
}

void monitoringDataCallback(void* target,
                            Kitsunemimi::Sakura::Session*,
                            const void* data,
                            const uint64_t dataSize)
{
    Gateway* gateway = static_cast<Gateway*>(target);
    WebSocketServer* server = gateway->m_websocketServer;
    const std::string text(static_cast<const char*>(data), dataSize);

    for(uint64_t i = 0; i < server->m_activeMonitoringSessions.size(); i++) {
        server->m_activeMonitoringSessions.at(i)->sendText(text);
    }
}

void sessionCallback(void* target,
                     bool isInit,
                     Kitsunemimi::Sakura::MessagingClient* session,
                     const std::string identifier)
{
    if(isInit)
    {
        if(identifier == "client") {
            session->setStreamMessageCallback(target, &clientDataCallback);
        }
        if(identifier == "monitoring") {
            session->setStreamMessageCallback(target, &monitoringDataCallback);
        }
    }
}

#endif // CALLBACKS_H
