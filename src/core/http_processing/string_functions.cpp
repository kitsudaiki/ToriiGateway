/**
 * @file        string_functions.cpp
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

#include "string_functions.h"

#include <libKitsunemimiCommon/methods/string_methods.h>
#include <libKitsunemimiJson/json_item.h>

/**
 * @brief precheck path
 *
 * @param path file-path to check
 *
 * @return false, if path is invalid, else true
 */
bool
checkPath(const std::string &path)
{
    if(path.empty()
            || path[0] != '/'
            || path.find("..") != std::string::npos)
    {
        return false;
    }

    return true;
}

/**
 * @brief parse uri
 *
 * @param target reference for teh target, which has to be parsed from the uri
 * @param token jwt-token to add the the request
 * @param request reference for the request, which should be filled by values from the uri
 * @param uri uri to parse
 * @param error reference for error-output
 *
 * @return true, if successful, else false
 */
bool
parseUri(std::string &target,
         const std::string &token,
         Kitsunemimi::Hanami::RequestMessage &request,
         const std::string &uri,
         Kitsunemimi::ErrorContainer &error)
{
    // first split of uri
    Kitsunemimi::Json::JsonItem parsedInputValues;
    std::vector<std::string> parts;
    Kitsunemimi::splitStringByDelimiter(parts, uri, '?');

    // check split-result
    if(parts.size() == 0)
    {
        error.addMeesage("Uri is empty.");
        return false;
    }
    if(parts.at(0).find("/") == std::string::npos)
    {
        error.addMeesage("Uri doesn't start with '/'.");
        return false;
    }

    if(parsedInputValues.parse(request.inputValues, error) == false)
    {
        error.addMeesage("Failed to parse input-values.");
        return false;
    }

    // split first part again to get target and real part
    const size_t cutPos = parts.at(0).find("/");
    const std::string* mainPart = &parts[0];
    target = mainPart->substr(0, cutPos);
    request.id = mainPart->substr(cutPos + 1, mainPart->size() - 1);

    // prepare payload, if exist
    if(parts.size() > 1)
    {
        std::vector<std::string> kvPairs;
        Kitsunemimi::splitStringByDelimiter(kvPairs, parts[1], '&');

        for(const std::string &kvPair : kvPairs)
        {
            const size_t cutPos = kvPair.find('=');
            const std::string key = kvPair.substr(0, cutPos);
            const std::string val = kvPair.substr(cutPos + 1, kvPair.size() - 1);
            parsedInputValues.insert(key, val, true);
        }
    }

    // add token to list of normal values
    parsedInputValues.insert("token", token);

    request.inputValues = parsedInputValues.toString();

    return true;
}

/**
 * @brief cut first part of a string, if match
 *
 * @param path reference for the incoming path and updated path
 * @param cut part, which have to match at the beginning of the path
 *
 * @return true, if first part match, else false
 */
bool
cutPath(std::string &path, const std::string &cut)
{
    if(path.compare(0, cut.size(), cut) == 0)
    {
        path.erase(0, cut.size());
        return true;
    }

    return false;
}
