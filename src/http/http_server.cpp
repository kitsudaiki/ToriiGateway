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

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ioc, {address, m_port}};
        for(;;)
        {
            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
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
