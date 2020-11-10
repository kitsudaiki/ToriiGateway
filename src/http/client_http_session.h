#ifndef CLIENT_HTTP_SESSION_H
#define CLIENT_HTTP_SESSION_H

#include <http/http_session.h>

class ClientHttpSession
        : public HttpSession
{
public:
    ClientHttpSession(tcp::socket &&socket);

protected:
    void processGetRequest();
    void processPostRequest();
    void processPutRequest();
    void processDelesteRequest();
};

#endif // CLIENT_HTTP_SESSION_H
