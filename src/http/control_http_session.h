#ifndef CONTROL_HTTP_SESSION_H
#define CONTROL_HTTP_SESSION_H

#include <http/http_session.h>

class ControlHttpSession
        : public HttpSession
{
public:
    ControlHttpSession(tcp::socket &&socket);

protected:
    void processGetRequest();
    void processPostRequest();
    void processPutRequest();
    void processDelesteRequest();
};

#endif // CONTROL_HTTP_SESSION_H
