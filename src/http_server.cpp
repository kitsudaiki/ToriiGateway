#include "http_server.h"

#include <libKitsunemimiCommon/common_items/data_items.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

using Kitsunemimi::Sakura::MessagingController;

HttpConnection::HttpConnection(tcp::socket socket)
    : m_socket(std::move(socket))
{
}

/**
 * @brief HttpConnection::start
 */
void
HttpConnection::start()
{
    readRequest();
    checkTimeout();
}

/**
 * @brief HttpConnection::readRequest
 */
void
HttpConnection::readRequest()
{
    auto self = shared_from_this();
    http::async_read(m_socket,
                     m_buffer,
                     m_request,
                     [self](beast::error_code ec, std::size_t bytes_transferred)
                     {
                         boost::ignore_unused(bytes_transferred);
                         if(!ec) {
                             self->processRequest();
                         }
                     });
}

/**
 * @brief HttpConnection::processRequest
 */
void
HttpConnection::processRequest()
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
HttpConnection::createResponse()
{
    if(m_request.target() == "/count")
    {
        Kitsunemimi::Sakura::MessagingClient* client = nullptr;
        client = MessagingController::getInstance()->getClient("ToriiGateway");
        std::string errorMessage = "";
        Kitsunemimi::DataMap inputValues;
        inputValues.insert("input", new Kitsunemimi::DataValue(42));
        inputValues.insert("test_output", new Kitsunemimi::DataValue(""));

        Kitsunemimi::DataMap resultingItem;
        client->triggerSakuraFile(resultingItem,
                                  "test-tree",
                                  inputValues.toString(),
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
HttpConnection::sendResponse()
{
    auto self = shared_from_this();
    m_response.content_length(m_response.body().size());
    http::async_write(m_socket,
                      m_response,
                      [self](beast::error_code ec, std::size_t)
                      {
                          self->m_socket.shutdown(tcp::socket::shutdown_send, ec);
                          self->m_deadline.cancel();
                      });
}

/**
 * @brief HttpConnection::checkTimeout
 */
void
HttpConnection::checkTimeout()
{
    auto self = shared_from_this();
    m_deadline.async_wait([self](beast::error_code ec)
                          {
                              if(!ec) {
                                  self->m_socket.close(ec);
                              }
                          });
}

/**
 * @brief HttpServer::HttpServer
 * @param url
 * @param port
 */
HttpServer::HttpServer(const std::string &ip, const uint16_t port)
{
    m_ip = ip;
    m_port = port;
}

/**
 * @brief HttpServer::~HttpServer
 */
HttpServer::~HttpServer() {}

/**
 * @brief http_server
 * @param acceptor
 * @param socket
 */
void
http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
                          [&](beast::error_code ec)
                          {
                              if(!ec) {
                                  std::make_shared<HttpConnection>(std::move(socket))->start();
                              }
                              http_server(acceptor, socket);
                          });
}

/**
 * @brief HttpServer::startListener
 * @return
 */
bool
HttpServer::startListener()
{
    try
    {
        const net::ip::address address = net::ip::make_address(m_ip);

        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};
        tcp::socket socket{ioc};
        http_server(acceptor, socket);

        ioc.run();
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void
HttpServer::closeListener()
{
}

