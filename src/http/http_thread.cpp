/**
 * @file        http_thread.cpp
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

#include "http_thread.h"

#include <gateway.h>
#include <http/request_queue.h>
#include <http/http_processing/http_session.h>

#include <libKitsunemimiCommon/threading/event.h>

/**
 * @brief constructor
 */
HttpThread::HttpThread(const std::string &threadName)
    : Kitsunemimi::Thread(threadName) {}

/**
 * @brief HttpThread::run
 */
void
HttpThread::run()
{
    while(m_abort == false)
    {
        Kitsunemimi::Event* event = Gateway::requestQueue->getRequest();
        if(event != nullptr)
        {
            event->processEvent();
            delete event;
        }
        else
        {
            sleepThread(10000);
        }
    }
}

