#include "http_server.h"

#include <http/http_session.h>

HttpServer::HttpServer(const std::string &address,
                       const uint16_t port)
{
    m_address = address;
    m_port = port;
}

void
HttpServer::run()
{
    try
    {
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};

        while(m_abort == false)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            HttpSession* session = new HttpSession(std::move(socket));
            session->startThread();
            m_activeSession.push_back(session);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
