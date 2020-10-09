#include "httpServer.h"

HttpServer::HttpServer(std::string url) : http_listener(url)
{
    support(methods::GET, std::bind(&HttpServer::handle_get, this, std::placeholders::_1));
    support(methods::PUT, std::bind(&HttpServer::handle_put, this, std::placeholders::_1));
    support(methods::POST, std::bind(&HttpServer::handle_post, this, std::placeholders::_1));
    support(methods::DEL, std::bind(&HttpServer::handle_delete, this, std::placeholders::_1));
}

HttpServer::~HttpServer()
{

}

void
HttpServer::startListener()
{
    open().wait();
}

void
HttpServer::closeListener()
{
    close().wait();
}


void
HttpServer::handle_error(pplx::task<void>& t)
{
    try {
        t.get();
    } catch(...) {
        // Ignore the error, Log it if a logger is available
    }
}


void
HttpServer::handle_get(http_request message)
{
    std::cout<<"GET: "<<message.to_string()<<std::endl;
    std::string path = http::uri::decode(message.relative_uri().path());
    std::cout<<"path: "<<path<<std::endl;
    auto paths = http::uri::split_path(path);

    std::string rep = U("YEAH");
    message.reply(status_codes::OK, rep);
    return;
};


void
HttpServer::handle_post(http_request message)
{
    std::cout<<"POST: "<<message.to_string()<<std::endl;
    message.reply(status_codes::OK);
    return ;
};


void
HttpServer::handle_delete(http_request message)
{
    std::cout<<"DELETE: "<<message.to_string()<<std::endl;
    std::string rep = U("WRITE YOUR OWN DELETE OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};


void
HttpServer::handle_put(http_request message)
{
    std::cout<<"PUT: "<<message.to_string()<<std::endl;
    std::string rep = U("WRITE YOUR OWN PUT OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};
