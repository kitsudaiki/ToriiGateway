/**
 * @file        client_web_socket_session.h
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

#ifndef CLIENT_WEB_SOCKET_SESSION_H
#define CLIENT_WEB_SOCKET_SESSION_H

#include <websocket/web_socket_session.h>

class ClientWebSocketSession
        : public WebSocketSession
{
public:
    ClientWebSocketSession(tcp::socket &&socket);
};

#endif // CLIENT_WEB_SOCKET_SESSION_H
