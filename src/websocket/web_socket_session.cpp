#include "web_socket_session.h"

#include <libKitsunemimiSakuraMessaging/messaging_client.h>
#include <libKitsunemimiSakuraMessaging/messaging_controller.h>

WebSocketSession::WebSocketSession(tcp::socket &&socket)
    : m_webSocket(std::move(socket))
{

}

bool
WebSocketSession::sendText(const std::string &text)
{
    beast::flat_buffer buffer;
    m_webSocket.text(true);
    boost::beast::ostream(buffer) << text;
    m_webSocket.write(buffer.data());

    return true;
}

void
WebSocketSession::run()
{
    try
    {
        // Set a decorator to change the Server of the handshake
        m_webSocket.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");
            }));

        // Accept the websocket handshake
        m_webSocket.accept();

        while(m_abort == false)
        {
            beast::flat_buffer buffer;
            m_webSocket.read(buffer);
            m_client->sendStreamData(buffer.data().data(), buffer.data().size());

            //char* content = static_cast<char*>(buffer.data().data());
            //const std::string text(content, buffer.data().size());
            //sleepThread(100000);
        }
    }
    catch(const beast::system_error& se)
    {
        // This indicates that the session was closed
        if(se.code() != websocket::error::closed) {
            std::cerr << "Error: " << se.code().message() << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
