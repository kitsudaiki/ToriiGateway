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

#include <libKitsunemimiJson/json_item.h>

#include <libKitsunemimiSakuraNetwork/session.h>

/**
 * @brief callback for stream-messages for the client
 *
 * @param target pointer to the Gateway-instance
 * @param data incoming data
 * @param dataSize number of incoming bytes
 */
void
clientDataCallback(void*,
                   Kitsunemimi::Sakura::Session*,
                   const void* data,
                   const uint64_t dataSize)
{
    const std::string text(static_cast<const char*>(data), dataSize);

}

/**
 * @brief forward data of a stream-message to a linked session
 *
 * @param target target-session to forward to
 * @param data data to farward
 * @param dataSize number of bytes to forward
 */
void
streamForwardCallback(void* target,
                      Kitsunemimi::Sakura::Session*,
                      const void* data,
                      const uint64_t dataSize)
{
    if(target == nullptr) {
        return;
    }

    Kitsunemimi::ErrorContainer error;
    Kitsunemimi::Sakura::Session* tSession = static_cast<Kitsunemimi::Sakura::Session*>(target);
    tSession->sendStreamData(data, dataSize, error);
}


void genericCallback(Kitsunemimi::Sakura::Session*,
                     const Kitsunemimi::Json::JsonItem&,
                     const uint64_t)
{

}
