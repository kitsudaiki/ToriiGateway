#include "http_session.h"

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/common_items/data_items.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

using Kitsunemimi::Sakura::MessagingController;

HttpSession::HttpSession(tcp::socket &&socket)
    : m_socket(std::move(socket))
{
}

/**
 * @brief HttpConnection::processRequest
 */
void
HttpSession::processRequest()
{
    m_response.version(m_request.version());
    m_response.keep_alive(false);

    switch(m_request.method())
    {
        case http::verb::get:
        {
            m_response.result(http::status::ok);
            m_response.set(http::field::server, "Beast");
            createResponse();
            break;
        }

        default:
        {
            // We return responses indicating an error if
            // we do not recognize the request method.
            m_response.result(http::status::bad_request);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body())
                << "Invalid request-method '"
                << std::string(m_request.method_string())
                << "'";
            break;
        }
    }

    sendResponse();
}

/**
 * @brief HttpConnection::createResponse
 * @param resp
 */
void
HttpSession::createResponse()
{
    if(m_request.target() == "/count")
    {
        m_client = MessagingController::getInstance()->getClient("control");
        std::string errorMessage = "";
        Kitsunemimi::DataMap inputValues;
        inputValues.insert("input", new Kitsunemimi::DataValue(42));
        inputValues.insert("test_output", new Kitsunemimi::DataValue(""));

        Kitsunemimi::DataMap resultingItem;
        m_client->triggerSakuraFile(resultingItem,
                                    "test-tree",
                                    inputValues,
                                    errorMessage);


        m_response.set(http::field::content_type, "text/json");
        beast::ostream(m_response.body()) << resultingItem.toString();
    }
    else if(m_request.target() == "/time")
    {
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body())
            <<  "<html>\n"
            <<  "</html>\n";
    }
    else
    {
        m_response.result(http::status::not_found);
        m_response.set(http::field::content_type, "text/plain");
        beast::ostream(m_response.body()) << "File not found\r\n";
    }
}

/**
 * @brief HttpConnection::sendResponse
 * @param resp
 */
void
HttpSession::sendResponse()
{
    beast::error_code ec;
    m_response.content_length(m_response.body().size());
    http::write(m_socket, m_response, ec);
}

/**
 * @brief HttpSession::run
 */
void
HttpSession::run()
{
    bool close = false;
    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    while(m_abort == false)
    {
        // Read a request
        http::read(m_socket, buffer, m_request, ec);
        if(ec == http::error::end_of_stream) {
            break;
        }

        if(ec) {
            std::cerr << "read" << ": " << ec.message() << "\n";
        }

        processRequest();

        // Send the response
        if(ec) {
            std::cerr << "write" << ": " << ec.message() << "\n";
        }

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }

    // Send a TCP shutdown
    m_socket.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}
