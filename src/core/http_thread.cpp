/**
 * @file        http_thread.cpp
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

#include "http_thread.h"

#include <torii_root.h>
#include <core/http_server.h>
#include <core/http_processing/http_session.h>
#include <core/web_socket_session.h>
#include <core/http_processing/http_session.h>

#include <libKitsunemimiCommon/threading/event.h>

/**
 * @brief constructor
 */
HttpThread::HttpThread(const std::string &threadName)
    : Kitsunemimi::Thread(threadName) {}

/**
 * @brief HttpThread::run
 */
void
HttpThread::run()
{
    while(m_abort == false)
    {
        tcp::socket* socket = ToriiGateway::httpServer->getSocket();
        if(socket != nullptr)
        {
            do
            {
                beast::ssl_stream<tcp::socket&> stream{*socket,
                                                       std::ref(ToriiGateway::httpServer->m_ctx)};
                http::request<http::string_body> httpRequest;
                http::response<http::dynamic_body> httpResponse;

                // Perform the SSL handshake
                beast::error_code ec;
                stream.handshake(boost::asio::ssl::stream_base::server, ec);
                if(ec.failed())
                {
                    Kitsunemimi::ErrorContainer error;
                    error.addMeesage("SSL-Handshake failed!");
                    LOG_ERROR(error);
                }

                if(readMessage(stream, httpRequest) == false) {
                    break;
                }

                if(websocket::is_upgrade(httpRequest))
                {
                    // initialize session
                    const std::string name = "WebSocketSession";
                    WebSocketSession* session = new WebSocketSession(stream, name);
                    if(session->init(httpRequest) == false) {
                        break;
                    }

                    session->startThread();
                    while(session->isInit == false) {
                        usleep(10000);
                    }
                }
                else
                {
                    processRequest(httpRequest, httpResponse);
                    httpRequest.clear();

                    if(sendResponse(stream, httpResponse) == false) {
                        break;
                    }

                    // close socket gain
                    beast::error_code ec;
                    stream.shutdown(ec);
                }
            }
            while(false);

            delete socket;
        }
        else
        {
            sleepThread(10000);
        }
    }
}

/**
 * @brief read message into buffer
 *
 * @return true, if successful, else false
 */
bool
HttpThread::readMessage(beast::ssl_stream<tcp::socket&> &stream,
                        http::request<http::string_body> &httpRequest)
{
    beast::error_code ec;
    beast::flat_buffer buffer;
    http::read(stream, buffer, httpRequest, ec);

    if(ec == http::error::end_of_stream) {
         return true;
    }

    if(ec)
    {
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("read: " + ec.message());
        LOG_ERROR(error);
        return false;
    }

    return true;
}

/**
 * @brief send prebuild response
 *
 * @return true, if successful, else false
 */
bool
HttpThread::sendResponse(beast::ssl_stream<tcp::socket&> &stream,
                         http::response<http::dynamic_body> &httpResponse)
{
    beast::error_code ec;
    httpResponse.content_length(httpResponse.body().size());
    http::write(stream, httpResponse, ec);

    if(ec)
    {
        Kitsunemimi::ErrorContainer error;
        error.addMeesage("write: " + ec.message());
        LOG_ERROR(error);
        return false;
    }

    return true;
}

