#include "client_web_socket_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

ClientWebSocketSession::ClientWebSocketSession(tcp::socket &&socket)
    : WebSocketSession(std::move(socket))
{
    m_client = MessagingController::getInstance()->getClient("client");
}
