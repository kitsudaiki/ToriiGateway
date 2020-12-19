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

#include <gateway.h>

#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

/**
 * @brief constructor
 */
ControlHttpSession::ControlHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = Gateway::m_instance->getClient("control");
}

/**
 * @brief process GET request
 *
 * @return always true
 */
bool
ControlHttpSession::processGetRequest()
{
    processRequest("{}");
    return true;
}

/**
 * @brief process POST request
 *
 * @return always false, because not supported at the moment
 */
bool
ControlHttpSession::processPostRequest()
{
    processRequest(m_request.body().data());
    return true;
}

/**
 * @brief process PUT request
 *
 * @return always false, because not supported at the moment
 */
bool
ControlHttpSession::processPutRequest()
{
    return false;
}

/**
 * @brief process DELTE request
 *
 * @return always false, because not supported at the moment
 */
bool
ControlHttpSession::processDelesteRequest()
{
    return false;
}

/**
 * @brief provess request
 *
 * @param inputValues json-formated input-values
 */
void
ControlHttpSession::processRequest(const std::string &inputValues)
{
    Kitsunemimi::DataMap result;
    std::string errorMessage = "";

    const std::string falseId = std::string(m_request.target().data(), m_request.target().size());
    const std::string correctId = falseId.substr(1, falseId.size()-1);

    // trigger sakura-file remote
    const bool ret = m_client->triggerSakuraFile(result,
                                                 correctId,
                                                 inputValues,
                                                 errorMessage);

    // forward result to the control
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
        LOG_ERROR(errorMessage);
        beast::ostream(m_response.body()) << errorMessage;
    }
}
