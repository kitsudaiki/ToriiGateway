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

void
ClientHttpSession::processGetRequest()
{

}

void
ClientHttpSession::processPostRequest()
{

}

void
ClientHttpSession::processPutRequest()
{

}

void
ClientHttpSession::processDelesteRequest()
{

}
