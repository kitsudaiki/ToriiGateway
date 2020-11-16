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
    if(m_request.target() == "/websocket") {
        assert(sendConnectionInfo("monitoring", "websocket_port"));
    } else if(m_request.target() == "/control") {
        assert(sendConnectionInfo("control", "http_port"));
    } else {
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
