#include "string_functions.h"

#include <libKitsunemimiCommon/common_methods/string_methods.h>
#include <libKitsunemimiJson/json_item.h>

/**
 * @brief checkPath
 * @param path
 * @return
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
 * @brief parseUri
 * @param path
 * @param inputValues
 * @param uri
 * @return
 */
bool
parseUri(std::string &target,
         std::string &token,
         Kitsunemimi::Hanami::RequestMessage &request,
         const std::string &uri,
         std::string &errorMessage)
{
    // first split of uri
    Kitsunemimi::Json::JsonItem parsedInputValues;
    std::vector<std::string> parts;
    Kitsunemimi::splitStringByDelimiter(parts, uri, '?');
    std::string key;
    std::string val;

    // check split-result
    if(parts.size() == 0) {
        return false;
    }
    if(parts.at(0).find("/") == std::string::npos) {
        return false;
    }

    if(parsedInputValues.parse(request.inputValues, errorMessage) == false) {
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
            key = kvPair.substr(0, cutPos);
            val = kvPair.substr(cutPos + 1, kvPair.size() - 1);
            parsedInputValues.insert(key, val, true);
        }
    }

    request.inputValues = parsedInputValues.toString();
    token = parsedInputValues.get("token").getString();

    return true;
}

/**
 * @brief cutPath
 * @param path
 * @param cut
 * @return
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
