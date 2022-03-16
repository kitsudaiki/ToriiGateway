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
#include <core/http_processing/http_processing.h>
#include <core/web_socket_session.h>
#include <core/http_processing/http_processing.h>

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
            Kitsunemimi::ErrorContainer error;

            if(handleSocket(socket, error) == false) {
                LOG_ERROR(error);
            }
            delete socket;
        }
        else
        {
            sleepThread(10000);
        }
    }
}

/**
 * @brief handle new incoming http-connection
 *
 * @param socket pointer to new socket to process
 * @param error reference for error-output
 *
 * @return true, if successful, else false
 */
bool
HttpThread::handleSocket(tcp::socket* socket,
                         Kitsunemimi::ErrorContainer &error)
{
    beast::ssl_stream<tcp::socket&>* stream = nullptr;
    stream = new beast::ssl_stream<tcp::socket&>{*socket, std::ref(ToriiGateway::httpServer->m_ctx)};
    http::request<http::string_body>* httpRequest = nullptr;
    httpRequest = new http::request<http::string_body>();
    http::response<http::dynamic_body> httpResponse;

    // perform the SSL handshake
    beast::error_code ec;
    stream->handshake(boost::asio::ssl::stream_base::server, ec);
    if(ec.failed())
    {
        error.addMeesage("SSL-Handshake failed while receiving new http-connection");
        return false;
    }

    // read http-message
    if(readMessage(*stream, *httpRequest, error) == false)
    {
        error.addMeesage("Can not send http-response.");
        return false;
    }

    // check if request belongs to a new websocket-request
    if(websocket::is_upgrade(*httpRequest))
    {
        // initialize new websocket-session
        const std::string name = "WebSocketSession";
        WebSocketSession* session = new WebSocketSession(stream, name);
        if(session->init(httpRequest) == false)
        {
            error.addMeesage("Can not init websocket.");
            return false;
        }

        // start new session-thread and wait until it reach its processing-loop to ensure, that the
        // websocket is ready to receive messages
        session->startThread();
        while(session->isInit == false) {
            usleep(10000);
        }

        sleep(10);
    }
    else
    {
        // process request
        processRequest(*httpRequest, httpResponse);
        if(sendResponse(*stream, httpResponse, error) == false)
        {
            error.addMeesage("Can not send http-response.");
            return false;
        }

        // close socket gain
        beast::error_code ec;
        stream->shutdown(ec);
        delete httpRequest;
        delete stream;

        if(ec)
        {
            error.addMeesage("error while closing http-stream: " + ec.message());
            return false;
        }
    }

    return true;
}

/**
 * @brief read message into buffer
 *
 * @return true, if successful, else false
 */
bool
HttpThread::readMessage(beast::ssl_stream<tcp::socket&> &stream,
                        http::request<http::string_body> &httpRequest,
                        Kitsunemimi::ErrorContainer &error)
{
    beast::error_code ec;
    beast::flat_buffer buffer;
    http::read(stream, buffer, httpRequest, ec);

    if(ec == http::error::end_of_stream) {
         return true;
    }

    if(ec)
    {
        error.addMeesage("error while read: " + ec.message());
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
                         http::response<http::dynamic_body> &httpResponse,
                         Kitsunemimi::ErrorContainer &error)
{
    beast::error_code ec;
    httpResponse.content_length(httpResponse.body().size());
    http::write(stream, httpResponse, ec);

    if(ec)
    {
        error.addMeesage("error while write: " + ec.message());
        return false;
    }

    return true;
}

