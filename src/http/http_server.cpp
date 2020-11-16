#include "http_server.h"

#include <http/client_http_session.h>
#include <http/control_http_session.h>
#include <http/monitoring_http_session.h>

#include <libKitsunemimiPersistence/logger/logger.h>

HttpServer::HttpServer(const std::string &address,
                       const uint16_t port,
                       const std::string &type)
{
    m_address = address;
    m_port = port;
    m_type = type;
}

void
HttpServer::run()
{
    LOG_INFO("start HTTP-server on address " + m_address + " and port " + std::to_string(m_port));
    try
    {
        const net::ip::address address = net::ip::make_address(m_address);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, m_port}};

        while(m_abort == false)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            HttpSession* session = nullptr;
            if(m_type == "client") {
                session = new ClientHttpSession(std::move(socket));
            }
            if(m_type == "control") {
                session = new ControlHttpSession(std::move(socket));
            }
            if(m_type == "monitoring") {
                session = new MonitoringHttpSession(std::move(socket));
            }

            session->startThread();
            m_activeSession.push_back(session);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
