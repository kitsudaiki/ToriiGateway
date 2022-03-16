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

#ifndef TORIIGATEWAY_HTTP_SERVER_H
#define TORIIGATEWAY_HTTP_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include <deque>

#include <libKitsunemimiCommon/threading/thread.h>
#include <libKitsunemimiCommon/logger.h>

using tcp = boost::asio::ip::tcp;

class HttpThread;

class HttpServer
        : public Kitsunemimi::Thread
{
public:
    HttpServer(const std::string &address,
               const uint16_t port,
               const std::string &cert,
               const std::string &key);

    boost::asio::ssl::context m_ctx;

    tcp::socket* getSocket();
    void addSocket(tcp::socket* socket);

protected:
    void run();

private:
    const std::string m_address = "";
    const uint16_t m_port = 0;
    const std::string m_certFilePath = "";
    const std::string m_keyFilePath = "";

    std::deque<tcp::socket*> m_queue;
    std::mutex m_queueMutex;

    bool loadCertificates(boost::asio::ssl::context &ctx,
                          Kitsunemimi::ErrorContainer &error);
};

#endif // TORIIGATEWAY_HTTP_SERVER_H
