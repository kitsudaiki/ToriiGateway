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
#include <boost/beast/ssl.hpp>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include <libKitsunemimiCommon/threading/thread.h>
#include <libKitsunemimiCommon/logger.h>

class HttpThread;

class HttpServer
        : public Kitsunemimi::Thread
{
public:
    HttpServer(const std::string &address,
               const uint16_t port,
               const std::string &cert,
               const std::string &key);

protected:
    void run();

private:
    const std::string m_address = "";
    const uint16_t m_port = 0;
    const std::string m_cert = "";
    const std::string m_key = "";

    bool loadCertificates(boost::asio::ssl::context &ctx,
                          const std::string &certFile,
                          const std::string &keyFile,
                          Kitsunemimi::ErrorContainer &error);
};

#endif // HTTP_SERVER_H
