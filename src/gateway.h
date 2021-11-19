/**
 * @file        gateway.h
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

#ifndef GATEWAYSERVER_H
#define GATEWAYSERVER_H

#include <iostream>
#include <map>
#include <vector>
#include <libKitsunemimiCommon/logger.h>

class WebSocketServer;
class HttpServer;
class HttpThread;
class RequestQueue;

class Gateway
{
public:
    Gateway();

    bool initInternalSession(Kitsunemimi::ErrorContainer &error);
    bool initWebSocketServer();
    bool initHttpServer();

    WebSocketServer* websocketServer = nullptr;
    HttpServer* httpServer = nullptr;
    static RequestQueue* requestQueue;

private:
    std::vector<HttpThread*> m_threads;
};

#endif // GATEWAYSERVER_H
