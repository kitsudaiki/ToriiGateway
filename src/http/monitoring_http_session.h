#ifndef MONITORING_HTTP_SESSION_H
#define MONITORING_HTTP_SESSION_H

#include <http/http_session.h>

class MonitoringHttpSession
        : public HttpSession
{
public:
    MonitoringHttpSession(tcp::socket &&socket);

protected:
    void processGetRequest();
    void processPostRequest();
    void processPutRequest();
    void processDelesteRequest();
};

#endif // MONITORING_HTTP_SESSION_H
