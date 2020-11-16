#include "web_socket_server.h"

#include <websocket/client_web_socket_session.h>
#include <websocket/monitoring_web_socket_session.h>

#include <libKitsunemimiPersistence/logger/logger.h>

WebSocketServer::WebSocketServer(const std::string &address,
                                 const uint16_t port,
                                 const std::string &type)
{
    m_address = address;
    m_port = port;
    m_type = type;
}

void
WebSocketServer::run()
{
    LOG_INFO("start Websocket-server on address " + m_address
             + " and port " + std::to_string(m_port));
    try
    {
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};

        while(m_abort == false)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            WebSocketSession* session = nullptr;
            if(m_type == "client")
            {
                session = new ClientWebSocketSession(std::move(socket));
                m_activeClientSessions.push_back(session);
            }
            if(m_type == "monitoring")
            {
                session = new MonitoringWebSocketSession(std::move(socket));
                m_activeMonitoringSessions.push_back(session);
            }

            session->startThread();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
