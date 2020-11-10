#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/filesystem.hpp>

#include <libKitsunemimiCommon/threading/thread.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Kitsunemimi {
namespace Sakura {
class MessagingClient;
class MessagingController;
}
}

using Kitsunemimi::Sakura::MessagingController;

class HttpSession
        : public Kitsunemimi::Thread
{
public:
    HttpSession(tcp::socket &&socket);

protected:
    void run();

    tcp::socket m_socket;
    std::string m_fileLocation = "";
    Kitsunemimi::Sakura::MessagingClient* m_client = nullptr;
    beast::flat_buffer m_buffer{8192};
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;

    void processRequest();
    void sendFileFromLocalLocation();
    void sendResponse();

    virtual void processGetRequest() = 0;
    virtual void processPostRequest() = 0;
    virtual void processPutRequest() = 0;
    virtual void processDelesteRequest() = 0;
};

#endif // HTTP_SESSION_H
