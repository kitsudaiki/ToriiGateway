#include "web_socket_session.h"

WebSocketSession::WebSocketSession(tcp::socket &&socket)
    : m_webSocket(std::move(socket))
{

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
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
            }));

        // Accept the websocket handshake
        m_webSocket.accept();

        for(;;)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            m_webSocket.read(buffer);

            char* content = static_cast<char*>(buffer.data().data());
            const std::string text(content, buffer.data().size());
            std::cout<<"poi: "<<text<<std::endl;
            // Echo the message back
            m_webSocket.text(m_webSocket.got_text());
            m_webSocket.write(buffer.data());
        }
    }
    catch(beast::system_error const& se)
    {
        // This indicates that the session was closed
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
