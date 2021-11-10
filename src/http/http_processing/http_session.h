/**
 * @file        http_session.h
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

#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <filesystem>

#include <libKitsunemimiCommon/threading/event.h>
#include <libKitsunemimiHanamiCommon/enums.h>
#include <libKitsunemimiHanamiCommon/structs.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using Kitsunemimi::Hanami::HttpRequestType;
using Kitsunemimi::Hanami::HttpResponseTypes;

namespace Kitsunemimi {
namespace Json {
class JsonItem;
}
}

class HttpRequestEvent
        : public Kitsunemimi::Event
{
public:
    HttpRequestEvent(tcp::socket &&socket,
                     boost::asio::ssl::context &ctx);

    bool processEvent();

protected:
    void run();

private:
    struct UriContanet
    {
        std::string target = "";
        std::string path = "";
        std::string inputValues = "";
        bool containsValues = false;
    };

    tcp::socket m_socket;
    beast::ssl_stream<tcp::socket&> m_stream;
    http::request<http::string_body> m_httpRequest;
    http::response<http::dynamic_body> m_httpResponse;

    void processRequest();
    bool sendConnectionInfo(const std::string &client, const std::string &portName);

    bool readMessage();
    bool sendResponse();

    bool requestToken(const std::string &target,
                      Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                      Kitsunemimi::Hanami::ResponseMessage &hanamiResponse,
                      std::string &errorMessage);
    bool checkPermission(const std::string &token,
                         const std::string &component,
                         const Kitsunemimi::Hanami::RequestMessage &hanamiRequest,
                         Kitsunemimi::Hanami::ResponseMessage &responseMsg,
                         std::string &errorMessage);
    bool processControlRequest(const std::string &uri,
                               const std::string &inputValues,
                               HttpRequestType httpType);

};

#endif // HTTP_SESSION_H