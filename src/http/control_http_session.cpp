#include "control_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

ControlHttpSession::ControlHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("control");
}

void
ControlHttpSession::processGetRequest()
{

}

void
ControlHttpSession::processPostRequest()
{

}

void
ControlHttpSession::processPutRequest()
{

}

void
ControlHttpSession::processDelesteRequest()
{

}
