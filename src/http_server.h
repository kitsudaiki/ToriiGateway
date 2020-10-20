#ifndef HTTPSERVER_H
#define HTTPSERVER_H

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

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


class HttpConnection
        : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection(tcp::socket socket);

    void start();

private:
    tcp::socket m_socket;

    beast::flat_buffer m_buffer{8192};
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;
    net::steady_timer m_deadline{m_socket.get_executor(), std::chrono::seconds(60)};

    void readRequest();
    void processRequest();
    void createResponse();
    void sendResponse();
    void checkTimeout();
};

class HttpServer
{
public:
    HttpServer(const std::string &ip, const uint16_t port);
    virtual ~HttpServer();

    bool startListener();
    void closeListener();

private:
    std::string m_ip = "";
    uint16_t m_port = 8080;
};

#endif // HTTPSERVER_H
