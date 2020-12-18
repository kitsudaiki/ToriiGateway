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

/**
 * @brief callback for stream-messages for the client
 *
 * @param target pointer to the Gateway-instance
 * @param data incoming data
 * @param dataSize number of incoming bytes
 */
void
clientDataCallback(Kitsunemimi::Sakura::Session*,
                   const void* data,
                   const uint64_t dataSize)
{
    Gateway* gateway = Gateway::m_instance;
    WebSocketServer* server = gateway->m_clientWebsocketServer;
    const std::string text(static_cast<const char*>(data), dataSize);

    // forward content to client
    WebSocketSession* session = server->getClientSession();
    if(session != nullptr) {
        session->sendText(text);
    }
}

/**
 * @brief callback for stream-messages for the monitoring
 *
 * @param target pointer to the Gateway-instance
 * @param data incoming data
 * @param dataSize number of incoming bytes
 */
void
monitoringDataCallback(Kitsunemimi::Sakura::Session*,
                       const void* data,
                       const uint64_t dataSize)
{
    Gateway* gateway = Gateway::m_instance;
    WebSocketServer* server = gateway->m_monitoringWebsocketServer;
    const std::string text(static_cast<const char*>(data), dataSize);

    // forward content to monitoring
    WebSocketSession* session = server->getMonitoringSession();
    if(session != nullptr) {
        session->sendText(text);
    }
}

/**
 * @brief callback for new sessions
 *
 * @param session pointer to session
 * @param identifier identifier of the incoming session
 */
void
messagingCreateCallback(Kitsunemimi::Sakura::MessagingClient* session,
                        const std::string identifier)
{
    Gateway::m_instance->addClient(identifier, session);

    if(identifier == "client") {
        session->setStreamMessageCallback(&clientDataCallback);
    }
    if(identifier == "monitoring") {
        session->setStreamMessageCallback(&monitoringDataCallback);
    }
}

/**
 * @brief callback for closing sessions
 *
 * @param identifier identifier of the incoming session
 */
void
messagingCloseCallback(const std::string identifier)
{
    Gateway::m_instance->removeClient(identifier);
}

#endif // CALLBACKS_H
