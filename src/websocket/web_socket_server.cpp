#include "web_socket_server.h"

#include <websocket/web_socket_session.h>

WebSocketServer::WebSocketServer(const std::string &address,
                                 const uint16_t port)
{
    m_address = address;
    m_port = port;
}

void
WebSocketServer::run()
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

            WebSocketSession* session = new WebSocketSession(std::move(socket));
            session->startThread();
            m_activeSession.push_back(session);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
