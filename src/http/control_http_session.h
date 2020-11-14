#ifndef CONTROL_HTTP_SESSION_H
#define CONTROL_HTTP_SESSION_H

#include <http/http_session.h>

class ControlHttpSession
        : public HttpSession
{
public:
    ControlHttpSession(tcp::socket &&socket);

protected:
    bool processGetRequest();
    bool processPostRequest();
    bool processPutRequest();
    bool processDelesteRequest();
};

#endif // CONTROL_HTTP_SESSION_H
