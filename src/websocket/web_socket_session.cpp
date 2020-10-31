#include "web_socket_session.h"

WebSocketSession::WebSocketSession(tcp::socket &&socket)
    : m_webSocket(std::move(socket))
{

}

void
WebSocketSession::run()
{
    srand(time(NULL));
    std::string testString = "[";

    for(uint32_t i = 0; i < 25; i++)
    {
        for(uint32_t j = 0; j < 25; j++)
        {
            if(i != 0
                    || j != 0)
            {
                testString += ",";
            }
            testString += "[" + std::to_string(i)
                        + "," + std::to_string(j)
                        + "," + std::to_string(rand() % 200)
                        + "]";
        }
    }
    testString += "]";
    std::cout<<testString<<std::endl;

    try
    {
        // Set a decorator to change the Server of the handshake
        m_webSocket.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                             " websocket-server-sync");
            }));

        // Accept the websocket handshake
        m_webSocket.accept();

        while(m_abort == false)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            //m_webSocket.read(buffer);

            //char* content = static_cast<char*>(buffer.data().data());
            //const std::string text(content, buffer.data().size());
            //std::cout<<"poi: "<<text<<std::endl;
            // Echo the message back
            //m_webSocket.text(m_webSocket.got_text());
            //m_webSocket.write(buffer.data());
            m_webSocket.text(true);
            boost::beast::ostream(buffer) << testString;
            std::cout<<"send: "<<testString<<std::endl;
            m_webSocket.write(buffer.data());
            sleepThread(100000);
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
