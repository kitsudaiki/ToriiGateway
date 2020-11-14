#include "monitoring_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

#include <libKitsunemimiConfig/config_handler.h>

MonitoringHttpSession::MonitoringHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("monitoring");
    bool success = false;
    m_fileLocation = GET_STRING_CONFIG("monitoring", "location", success);
    assert(success);
}

bool
MonitoringHttpSession::processGetRequest()
{
    if(m_request.target() == "/websocket")
    {
        bool success = false;
        const uint16_t port = static_cast<uint16_t>(GET_INT_CONFIG("monitoring",
                                                                   "websocket_port",
                                                                   success));
        const std::string ip = GET_STRING_CONFIG("monitoring",
                                                 "ip",
                                                 success);

        const std::string result = "{\"port\":"
                                   + std::to_string(port)
                                   + ",\"ip\":\""
                                   + ip
                                   + "\"}";
        m_response.set(http::field::content_type, "text/json");
        beast::ostream(m_response.body()) << result;
    }
    else
    {
        sendFileFromLocalLocation();
    }

    return true;
}

bool
MonitoringHttpSession::processPostRequest()
{
    return false;
}

bool
MonitoringHttpSession::processPutRequest()
{
    return false;
}

bool
MonitoringHttpSession::processDelesteRequest()
{
    return false;
}
