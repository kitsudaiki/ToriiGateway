#include "monitoring_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

MonitoringHttpSession::MonitoringHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("monitoring");
}

void
MonitoringHttpSession::processGetRequest()
{

}

void
MonitoringHttpSession::processPostRequest()
{

}

void
MonitoringHttpSession::processPutRequest()
{

}

void
MonitoringHttpSession::processDelesteRequest()
{

}
