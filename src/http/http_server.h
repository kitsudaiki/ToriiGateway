#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

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

#include <libKitsunemimiCommon/threading/thread.h>

class HttpSession;

class HttpServer
        : public Kitsunemimi::Thread
{
public:
    HttpServer(const std::string &address,
               const uint16_t port,
               const std::string &type);

protected:
    void run();

private:
    std::string m_address = "";
    uint16_t m_port = 0;
    std::string m_type = "";

    std::vector<HttpSession*> m_activeSession;
};

#endif // HTTP_SERVER_H
