/**
 * @file        string_functions.h
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

#ifndef TORIIGATEWAY_STRING_FUNCTIONS_H
#define TORIIGATEWAY_STRING_FUNCTIONS_H

#include <string>
#include <libKitsunemimiHanamiCommon/structs.h>
#include <libKitsunemimiCommon/logger.h>

using Kitsunemimi::Hanami::HttpRequestType;
using Kitsunemimi::Hanami::HttpResponseTypes;

namespace Kitsunemimi {
namespace Json {
class JsonItem;
}
}

bool checkPath(const std::string &path);
bool parseUri(std::string &target,
              const std::string &token,
              Kitsunemimi::Hanami::RequestMessage &request,
              const std::string &uri,
              Kitsunemimi::ErrorContainer &error);
bool cutPath(std::string &path,
             const std::string &cut);

#endif // TORIIGATEWAY_STRING_FUNCTIONS_H
