#include "client_http_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

ClientHttpSession::ClientHttpSession(tcp::socket &&socket)
    : HttpSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("client");
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
