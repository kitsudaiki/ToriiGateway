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

#include <http/http_processing/http_session.h>
#include <libKitsunemimiCommon/logger.h>

/**
 * @brief constructor
 */
RequestQueue::RequestQueue() {}

/**
 * @brief get request from the queue
 *
 * @return request
 */
HttpRequestEvent*
RequestQueue::getRequest()
{
    std::lock_guard<std::mutex> guard(m_queueMutex);

    HttpRequestEvent* result = nullptr;
    if(m_queue.size() > 0)
    {
        result = m_queue.front();
        m_queue.pop_front();
    }

    return result;
}

/**
 * @brief add request to queue
 *
 * @param newRequest new request for the queue
 */
void
RequestQueue::addRequest(HttpRequestEvent* newRequest)
{
    std::lock_guard<std::mutex> guard(m_queueMutex);
    m_queue.push_back(newRequest);
}
