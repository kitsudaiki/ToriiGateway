/**
 * @file        http_server.h
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

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include <libKitsunemimiCommon/threading/thread.h>

class HttpSession;

class HttpServer
        : public Kitsunemimi::Thread
{
public:
    HttpServer(const std::string &address,
               const uint16_t port,
               const std::string &type);

protected:
    void run();

private:
    std::string m_address = "";
    uint16_t m_port = 0;
    std::string m_type = "";

    std::vector<HttpSession*> m_activeSession;
};

#endif // HTTP_SERVER_H
