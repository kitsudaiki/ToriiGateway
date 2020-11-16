#ifndef MONITORING_HTTP_SESSION_H
#define MONITORING_HTTP_SESSION_H

#include <http/http_session.h>

class MonitoringHttpSession
        : public HttpSession
{
public:
    MonitoringHttpSession(tcp::socket &&socket);

protected:
    bool processGetRequest();
    bool processPostRequest();
    bool processPutRequest();
    bool processDelesteRequest();
};

#endif // MONITORING_HTTP_SESSION_H
