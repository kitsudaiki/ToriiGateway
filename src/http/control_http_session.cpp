/**
 * @file        control_http_session.cpp
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

#include "control_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

ControlHttpSession::ControlHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("control");
}

bool
ControlHttpSession::processGetRequest()
{
    Kitsunemimi::DataMap result;
    std::string errorMessage = "";

    const bool ret = m_client->triggerSakuraFile(result,
                                                 m_request.target().data(),
                                                 m_request.body().data(),
                                                 errorMessage);

    if(ret)
    {
        m_response.result(http::status::ok);
        m_response.set(http::field::content_type, "text/json");
        beast::ostream(m_response.body()) << result.toString();
    }
    else
    {
        m_response.result(http::status::not_found);
        m_response.set(http::field::content_type, "text/plain");
        beast::ostream(m_response.body()) << errorMessage;
    }

    return true;
}

bool
ControlHttpSession::processPostRequest()
{
    return false;
}

bool
ControlHttpSession::processPutRequest()
{
    return false;
}

bool
ControlHttpSession::processDelesteRequest()
{
    return false;
}
