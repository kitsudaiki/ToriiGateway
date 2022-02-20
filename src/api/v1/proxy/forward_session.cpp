/**
 * @file        forward_session.cpp
 *
 * @author      Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright   Apache License Version 2.0
 *
 *      Copyright 2021 Tobias Anker
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

#include "forward_session.h"

#include <libKitsunemimiHanamiMessaging/hanami_messaging.h>
#include <libKitsunemimiHanamiMessaging/hanami_messaging_client.h>
#include <libKitsunemimiHanamiCommon/enums.h>

#include <callbacks.h>

using namespace Kitsunemimi;

ForwardSession::ForwardSession()
    : Kitsunemimi::Sakura::Blossom("Connect an incoming session and an outgoing session with "
                                   "each other in one direction to forward messages coming from "
                                   "the outside to the backend.")
{
    //----------------------------------------------------------------------------------------------
    // input
    //----------------------------------------------------------------------------------------------

    registerInputField("source_name",
                       Sakura::SAKURA_STRING_TYPE,
                       true,
                       "Name of the source-session.");
    assert(addFieldBorder("source_name", 4, 256));

    registerInputField("target_name",
                       Sakura::SAKURA_STRING_TYPE,
                       true,
                       "Name of the target-session.");
    assert(addFieldBorder("target_name", 4, 256));

    //----------------------------------------------------------------------------------------------
    //
    //----------------------------------------------------------------------------------------------
}

/**
 * @brief runTask
 */
bool
ForwardSession::runTask(Sakura::BlossomLeaf &blossomLeaf,
                        const Kitsunemimi::DataMap &,
                        Sakura::BlossomStatus &status,
                        ErrorContainer &error)
{
    const std::string sourceName = blossomLeaf.input.get("source_name").getString();
    const std::string targetName = blossomLeaf.input.get("target_name").getString();

    Hanami::HanamiMessaging* messageInterface = Hanami::HanamiMessaging::getInstance();

    // get source-session by name
    Hanami::HanamiMessagingClient* sourceSession = messageInterface->getIncomingClient(sourceName);
    if(sourceSession == nullptr)
    {
        status.statusCode = Hanami::NOT_FOUND_RTYPE;
        status.errorMessage = "source-session with name '" + sourceName + "' was not found.";
        error.addMeesage(status.errorMessage);
        return false;
    }

    // get target-session by name
    Hanami::HanamiMessagingClient* targetSession = messageInterface->getOutgoingClient(targetName);
    if(targetSession == nullptr)
    {
        status.statusCode = Hanami::NOT_FOUND_RTYPE;
        status.errorMessage = "target-session with name '" + targetName + "' was not found.";
        error.addMeesage(status.errorMessage);
    }

    return sourceSession->setStreamCallback(targetSession, &streamForwardCallback);
}
