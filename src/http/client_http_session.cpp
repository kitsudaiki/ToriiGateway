#include "client_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

#include <libKitsunemimiConfig/config_handler.h>

ClientHttpSession::ClientHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("client");
    bool success = false;
    m_fileLocation = GET_STRING_CONFIG("client", "location", success);
    assert(success);
}

bool
ClientHttpSession::processGetRequest()
{
    if(m_request.target() == "/websocket") {
        assert(sendWebsocketInfo("client"));
    } else {
        sendFileFromLocalLocation();
    }

    return true;
}

bool
ClientHttpSession::processPostRequest()
{
    return false;
}

bool
ClientHttpSession::processPutRequest()
{
    return false;
}

bool
ClientHttpSession::processDelesteRequest()
{
    return false;
}
