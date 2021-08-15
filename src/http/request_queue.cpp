/**
 * @file        request_queue.cpp
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

#include "request_queue.h"

#include <http/http_session.h>

RequestQueue::RequestQueue() {}

/**
 * @brief RequestQueue::getSession
 * @return
 */
HttpRequestEvent*
RequestQueue::getSession()
{
    HttpRequestEvent* result = nullptr;
    if(m_queue.size() == 0) {
        return result;
    }

    m_queueMutex.lock();
    if(m_queue.size() >= 0)
    {
        result = m_queue.front();
        m_queue.pop_front();
    }
    m_queueMutex.unlock();

    return result;
}

/**
 * @brief RequestQueue::addSession
 * @param session
 */
void
RequestQueue::addSession(HttpRequestEvent *session)
{
    m_queueMutex.lock();
    m_queue.push_back(session);
    m_queueMutex.unlock();
}