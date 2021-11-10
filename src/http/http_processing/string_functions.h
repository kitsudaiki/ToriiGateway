#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <string>
#include <libKitsunemimiHanamiCommon/structs.h>

using Kitsunemimi::Hanami::HttpRequestType;
using Kitsunemimi::Hanami::HttpResponseTypes;

namespace Kitsunemimi {
namespace Json {
class JsonItem;
}
}

bool checkPath(const std::string &path);
bool parseUri(std::string &target,
              std::string &token,
              Kitsunemimi::Hanami::RequestMessage &request,
              const std::string &uri,
              std::string &errorMessage);
bool cutPath(std::string &path,
             const std::string &cut);

#endif // STRING_FUNCTIONS_H