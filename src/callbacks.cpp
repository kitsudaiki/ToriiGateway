/**
 * @file       callbacks.cpp
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

#include <callbacks.h>
#include <core/http_websocket_thread.h>

#include <libKitsunemimiJson/json_item.h>

#include <libKitsunemimiSakuraNetwork/session.h>

/**
 * @brief forward data of a stream-message to a linked session
 *
 * @param target target-session to forward to
 * @param data data to farward
 * @param dataSize number of bytes to forward
 */
void
streamDataCallback(void* receiver,
                   Kitsunemimi::Sakura::Session*,
                   const void* data,
                   const uint64_t dataSize)
{
    HttpWebsocketThread* webSocket = static_cast<HttpWebsocketThread*>(receiver);
    webSocket->sendData(data, dataSize);
}

/**
 * @brief unused placeholder
 */
void
genericCallback(Kitsunemimi::Sakura::Session*,
                const uint32_t,
                void*,
                const uint64_t,
                const uint64_t)
{}
