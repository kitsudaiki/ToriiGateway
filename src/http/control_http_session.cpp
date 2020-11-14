#include "control_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

ControlHttpSession::ControlHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("control");
}

bool
ControlHttpSession::processGetRequest()
{
    return false;
}

bool
ControlHttpSession::processPostRequest()
{
    return false;
}

bool
ControlHttpSession::processPutRequest()
{
    return false;
}

bool
ControlHttpSession::processDelesteRequest()
{
    return false;
}
