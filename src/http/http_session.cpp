#include "http_session.h"

#include <libKitsunemimiConfig/config_handler.h>
#include <libKitsunemimiCommon/common_items/data_items.h>
#include <libKitsunemimiPersistence/files/text_file.h>
#include <libKitsunemimiPersistence/logger/logger.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

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
    m_response.set(http::field::server, "ToriiGateway");
    m_response.result(http::status::ok);

    switch(m_request.method())
    {
        case http::verb::get:
        {
            if(processGetRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::post:
        {
            if(processPostRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::put:
        {
            if(processPutRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        case http::verb::delete_:
        {
            if(processDelesteRequest() == false)
            {
                m_response.result(http::status::not_found);
                m_response.set(http::field::content_type, "text/plain");
            }
            break;
        }

        default:
        {
            m_response.result(http::status::bad_request);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body())
                << "Invalid request-method '"
                << std::string(m_request.method_string())
                << "'";
            break;
        }
    }
}

/**
 * @brief HttpConnection::createResponse
 * @param resp
 */
bool
HttpSession::sendFileFromLocalLocation()
{
    // create file-path
    std::string path = m_fileLocation;
    path += m_request.target().to_string();
    if(m_request.target() == "/") {
        path += "index.html";
    }

    // set response-type based on file-type
    boost::filesystem::path pathObj(path);
    const std::string extension = pathObj.extension().string();
    if(pathObj.extension().string() == ".html") {
        m_response.set(http::field::content_type, "text/html");
    } else if(pathObj.extension().string() == ".css") {
        m_response.set(http::field::content_type, "text/css");
    } else {
        m_response.set(http::field::content_type, "text/plain");
    }

    // read file and send content back
    std::string fileContent = "";
    std::string errorMessage = "";
    if(Kitsunemimi::Persistence::readFile(fileContent, path, errorMessage))
    {
        beast::ostream(m_response.body()) << fileContent;
        return true;
    }

    m_response.result(http::status::internal_server_error);
    m_response.set(http::field::content_type, "text/plain");
    LOG_ERROR(errorMessage);

    return false;
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
        sendResponse();

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
