/**
 * @file        http_server.cpp
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

#include "http_server.h"

#include <gateway.h>

#include <http/http_session.h>
#include <http/http_thread.h>
#include <http/request_queue.h>

#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiCommon/files/text_file.h>

/**
 * @brief constructor
 *
 * @param address address to listen on
 * @param port port to listen
 */
HttpServer::HttpServer(const std::string &address,
                       const uint16_t port,
                       const std::string &cert,
                       const std::string &key)
    : m_address(address),
      m_port(port),
      m_cert(cert),
      m_key(key)
{
}

/**
 * @brief HttpServer::loadCertificates
 * @param ctx
 * @param cert
 * @param key
 */
bool
HttpServer::loadCertificates(boost::asio::ssl::context& ctx,
                             const std::string &certFile,
                             const std::string &keyFile)
{
    std::string errorMessage = "";
    std::string cert = "";
    std::string key = "";
    bool ret = false;

    ret = Kitsunemimi::readFile(cert, certFile, errorMessage);
    if(ret == false)
    {
        LOG_ERROR(errorMessage);
        return false;
    }

    ret = Kitsunemimi::readFile(key, keyFile, errorMessage);
    if(ret == false)
    {
        LOG_ERROR(errorMessage);
        return false;
    }

    const std::string dh = "-----BEGIN DH PARAMETERS-----\n"
                           "MIIBCAKCAQEArzQc5mpm0Fs8yahDeySj31JZlwEphUdZ9StM2D8+Fo7TMduGtSi+\n"
                           "/HRWVwHcTFAgrxVdm+dl474mOUqqaz4MpzIb6+6OVfWHbQJmXPepZKyu4LgUPvY/\n"
                           "4q3/iDMjIS0fLOu/bLuObwU5ccZmDgfhmz1GanRlTQOiYRty3FiOATWZBRh6uv4u\n"
                           "tff4A9Bm3V9tLx9S6djq31w31Gl7OQhryodW28kc16t9TvO1BzcV3HjRPwpe701X\n"
                           "oEEZdnZWANkkpR/m/pfgdmGPU66S2sXMHgsliViQWpDCYeehrvFRHEdR9NV+XJfC\n"
                           "QMUk26jPTIVTLfXmmwU0u8vUkpR7LQKkwwIBAg==\n"
                           "-----END DH PARAMETERS-----\n";


    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(boost::asio::buffer(key.data(), key.size()),
                        boost::asio::ssl::context::file_format::pem);

    ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));

    return true;
}

/**
 * @brief run server-thread
 */
void
HttpServer::run()
{
    LOG_INFO("start HTTP-server on address " + m_address + " and port " + std::to_string(m_port));
    try
    {
        // create server
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};
        boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
        const bool loadResult = loadCertificates(ctx, m_cert, m_key);
        if(loadResult == false) {
            return;
        }

        while(m_abort == false)
        {
            // create socket-object for incoming connection
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            // process http-request within an already existing thread
            HttpRequestEvent* event = new HttpRequestEvent(std::move(socket), std::ref(ctx));
            Gateway::m_requestQueue->addSession(event);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
