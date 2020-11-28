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
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include <libKitsunemimiCommon/threading/event.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Kitsunemimi {
namespace Sakura {
class MessagingClient;
class MessagingController;
}
}

using Kitsunemimi::Sakura::MessagingController;

class HttpSession
        : public Kitsunemimi::Event
{
public:
    HttpSession(tcp::socket &&socket);

    bool processEvent();

protected:
    void run();

    tcp::socket m_socket;
    std::string m_fileLocation = "";
    Kitsunemimi::Sakura::MessagingClient* m_client = nullptr;
    beast::flat_buffer m_buffer{8192};
    http::request<http::string_body> m_request;
    http::response<http::dynamic_body> m_response;

    void processRequest();
    bool sendFileFromLocalLocation();
    bool sendConnectionInfo(const std::string &client, const std::string &portName);
    bool sendControlInfo();

    void sendResponse();

    virtual bool processGetRequest() = 0;
    virtual bool processPostRequest() = 0;
    virtual bool processPutRequest() = 0;
    virtual bool processDelesteRequest() = 0;
};

#endif // HTTP_SESSION_H
