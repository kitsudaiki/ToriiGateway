#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>


#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class HttpServer : public http_listener
{
public:
    HttpServer(std::string url);
    virtual ~HttpServer();

    void startListener();
    void closeListener();

private:
    void handle_get(http_request message);
    void handle_put(http_request message);
    void handle_post(http_request message);
    void handle_delete(http_request message);
    void handle_error(pplx::task<void>& t);
};

#endif // HTTPSERVER_H
