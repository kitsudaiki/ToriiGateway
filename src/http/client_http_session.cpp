/**
 * @file        client_http_session.cpp
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

#include "client_http_session.h"

#include <gateway.h>

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

#include <libKitsunemimiConfig/config_handler.h>

/**
 * @brief constructor
 */
ClientHttpSession::ClientHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = Gateway::m_instance->getClient("client");
    bool success = false;
    m_fileLocation = GET_STRING_CONFIG("client", "location", success);
    assert(success);
}

/**
 * @brief process GET request
 *
 * @return always true
 */
bool
ClientHttpSession::processGetRequest()
{
    if(m_request.target() == "/websocket") {
        assert(sendConnectionInfo("client", "websocket_port"));
    } else if(m_request.target() == "/control") {
        assert(sendConnectionInfo("control", "http_port"));
    } else {
        sendFileFromLocalLocation();
    }

    return true;
}

/**
 * @brief process POST request
 *
 * @return always false, because not supported at the moment
 */
bool
ClientHttpSession::processPostRequest()
{
    return false;
}

/**
 * @brief process PUT request
 *
 * @return always false, because not supported at the moment
 */
bool
ClientHttpSession::processPutRequest()
{
    return false;
}

/**
 * @brief process DELTE request
 *
 * @return always false, because not supported at the moment
 */
bool
ClientHttpSession::processDelesteRequest()
{
    return false;
}
